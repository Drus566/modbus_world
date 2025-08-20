#include "DataManager.h"

#include "RegManager.h"
#include "MemManager.h"
#include "RangeManager.h"
#include "RequestManager.h"
#include "Logger.h"

#include "String.h"

namespace mb {
namespace data {

using namespace mb::log;

DataManager::DataManager(Config* config, MemManager* mem_manager) : m_config(config),
                                                                    m_mem_manager(mem_manager),
                                                                    m_print_settings(false),
                                                                    m_print_regs(false) {}

DataManager::~DataManager() {
   if (m_range_manager) delete m_range_manager;
   if (m_reg_manager) delete m_reg_manager;
   if (m_request_manager) delete m_request_manager;
}

bool DataManager::start() {
    Logger::Instance()->log(LogLevel::INFO, "Modbus data initialize...");

    bool result = false;

    m_print_settings = m_config->printSettings();
    m_print_regs = m_config->printRegs();

    int mcbr = m_config->maxCountRegsRead();
    if (mcbr <= 0) mcbr = DEFAULT_MAX_COUNT_REGS_READ;

    RegDataOrder data_order = RegDataOrder::NONE;
    std::string data_order_str = m_config->dataOrder();
    data_order_str = toUpperCase(data_order_str);
    if (!getRegDataOrderFromStr(data_order, data_order_str)) data_order = DEFAULT_DATA_ORDER;

    // Logger::Instance()->log(LogLevel::INFO, "Max count regs read %d", mcbr);

    m_range_manager = new RangeManager;
    m_reg_manager = new RegManager(data_order);
    m_request_manager = new RequestManager(mcbr);

    int slave_id = m_config->slaveId();

    SectionsMap map;
    map = m_config->getAreas(map);
    // if (map.empty()) {
        // Logger::Instance()->log(LogLevel::ERROR, "Section map is empty");
        // return false;
    // }

    if (!map.empty()) parseRanges(slave_id, map);
    // if (!result) {
    //     Logger::Instance()->log(LogLevel::ERROR, "Uncorrect parse ranges");
    //     return false;
    // }

    if (m_config->printRanges()) m_range_manager->printInfo();

    map.clear();
    map = m_config->getRegs(map);
    if (!map.empty()) {
        result = parseRegs(slave_id, map);
        if (!result) { 
            Logger::Instance()->log(LogLevel::ERROR, "Uncorrect parse regs");
            return false;
        }
    }

    createRequests();

    if (m_config->printRegs()) m_reg_manager->printInfo();
    if (m_config->printRequests()) m_request_manager->printInfo();
    
    return true;
}

bool DataManager::parseRanges(int slave_id, SectionsMap& map) {
    bool result = false;
    int current_slave_id;
    FuncNumber function;

    for (const auto& pair : map) {
        const std::string& section = pair.first;
        bool is_area_section = false;
        current_slave_id = slave_id; 

        /** Get function */
        if (startsWith(section, AREA_FUNCS_SECTION)) is_area_section = true;
        else if (startsWith(section, COILS_AREA_SECTION) || startsWith(section, F1_AREA_SECTION)) function = FuncNumber::READ_COIL;
        else if (startsWith(section, DISCRETE_AREA_SECTION) || startsWith(section, F2_AREA_SECTION)) function = FuncNumber::READ_INPUT_COIL;
        else if (startsWith(section, HOLDING_REGS_AREA_SECTION) || startsWith(section, F3_AREA_SECTION)) function = FuncNumber::READ_REGS;
        else if (startsWith(section, INPUT_REGS_AREA_SECTION) || startsWith(section, F4_AREA_SECTION)) function = FuncNumber::READ_INPUT_REGS;

        /** Get section */
        std::string pure_section;
        const std::vector<std::string> section_slave = split(section, '#');
        if (section_slave.size() > 1) {
            if (isNumber(section_slave[1])) current_slave_id = std::stoi(section_slave[1]);
            pure_section = section_slave[0];
        }
        else pure_section = section;
        
        /* Get params */
        const auto& params_list = map[section];
        for (const auto& params: params_list) {
            for (const auto& param : params) {
                std::string range_str = param.second;
                if (is_area_section) {
                    const std::string& param_key_str = param.first;
                    if (param_key_str == COILS_KEY) function = FuncNumber::READ_COIL;
                    else if (param_key_str == DISCRETE_INPUTS_KEY)   function = FuncNumber::READ_INPUT_COIL;
                    else if (param_key_str == INPUT_REGISTERS_KEY)   function = FuncNumber::READ_INPUT_REGS;
                    else if (param_key_str == HOLDING_REGISTERS_KEY) function = FuncNumber::READ_REGS;
                }
                m_range_manager->addRange(current_slave_id, function, range_str);
            }
        }
        result = true;
    }
    
    /** Normalize */
    m_range_manager->normalizeRanges();

    return result;
}

bool DataManager::parseRegs(int slave_id, SectionsMap& map) {
    bool result = false;
    bool is_describe = false;
    int current_slave_id;
    std::string current_section;
    FuncNumber function;
    
    char slave_delimeter = '#';
    char describe_delimeter = '|'; 

    for (const auto& pair : map) {
        is_describe = false;
        const std::string& section = pair.first;
        current_slave_id = slave_id; 

        /** Get function */
        if (startsWith(section, WRITE_WORD_SECTION)        || startsWith(section, F6_SECTION))  function = FuncNumber::WRITE_SINGLE_WORD;
        else if (startsWith(section, WRITE_WORDS_SECTION)  || startsWith(section, F16_SECTION)) function = FuncNumber::WRITE_MULTIPLE_WORDS;
        else if (startsWith(section, WRITE_COIL_SECTION)   || startsWith(section, F5_SECTION))  function = FuncNumber::WRITE_SINGLE_COIL;
        else if (startsWith(section, WRITE_COILS_SECTION)  || startsWith(section, F15_SECTION)) function = FuncNumber::WRITE_MULTIPLE_COILS;
        else if (startsWith(section, COILS_SECTION)        || startsWith(section, F1_SECTION))  function = FuncNumber::READ_COIL;
        else if (startsWith(section, DISCRETE_SECTION)     || startsWith(section, F2_SECTION))  function = FuncNumber::READ_INPUT_COIL;
        else if (startsWith(section, HOLDING_REGS_SECTION) || startsWith(section, F3_SECTION))  function = FuncNumber::READ_REGS;
        else if (startsWith(section, INPUT_REGS_SECTION)   || startsWith(section, F4_SECTION))  function = FuncNumber::READ_INPUT_REGS;

        current_section = section;

        // Check is describe ?
        const std::vector<std::string> describe_tokens = split(section, describe_delimeter);
        if (describe_tokens.size() > 1 && (describe_tokens[1] == D_KEY || describe_tokens[1] == DESCRIBE_KEY)) {
            is_describe = true;
            current_section = describe_tokens[0];
        } 

        /** Get section */
        std::string pure_section;
        const std::vector<std::string> section_slave = split(current_section, slave_delimeter);
        // Check slave
        if (section_slave.size() > 1) {            
            // Set slave id
            if (isNumber(section_slave[1])) current_slave_id = std::stoi(section_slave[1]);
            pure_section = section_slave[0];
        }
        else pure_section = current_section;

        /* Get params */
        const auto& params_list = map[section];
        for (const auto& params: params_list) {
            for (const auto& param : params) {
                std::string val = param.second;
                m_reg_manager->addReg(is_describe, current_slave_id, function, param.first, val);
            }
        }
        result = true;
    }
    
    /** Complete ranges with regs */
    completeRanges();

    return result;
}

void DataManager::completeRanges() {
    int start, end;
    std::forward_list<Register>& regs = m_reg_manager->getReadRegs();
    for (const auto& reg : regs) {
        start = reg.address;
        end = start;
        if (isDwordDataType(reg.reg_info.data_type)) end += 1;
        m_range_manager->addRange(reg.slave_id, reg.function, start, end);
    }
    m_range_manager->normalizeRanges();
}

void DataManager::createRequests() {
    int regs_read = m_request_manager->getMaxCountRegsRead();
    int common_quantity, group_count, end, offset, current_address;
    bool response;
    MemoryChunk* mem;

    if (regs_read == 0) return;

    /** Create read requests */
    const RangesMap& ranges_map = m_range_manager->getRanges();
    for (const auto& slave : ranges_map) {
        for (const auto& function : slave.second) {
            const int slave_id = slave.first;
            const FuncNumber func = function.first;
            const std::vector<Range>& ranges = function.second;

            for (const auto& range : ranges) {
                // общее количество считываемых регистров 
                common_quantity = (range.end - range.start) + 1;
                group_count = 1;
                end = 0;
                offset = 0;
                current_address = range.start;

                if (regs_read > 1) {
                    group_count = common_quantity / regs_read; 
                    end = common_quantity % regs_read;
                }

                if (isCoilFunc(func)) response = m_mem_manager->createU8Memory(mem, common_quantity);
                else response = m_mem_manager->createU16Memory(mem, common_quantity);
                
                if (response) {
                    for (int i = 0; i < group_count; i++) {
                        m_request_manager->addRequest(slave_id, func, current_address, regs_read, mem, offset);
                        offset += regs_read;
                        current_address += regs_read;
                    }
                    if (end > 0) m_request_manager->addRequest(slave_id, func, current_address, end, mem, offset);
                }
            }
        }
    }
}

Register* DataManager::findReadRegOnlyByName(const std::string& name) {
    Register* result = nullptr;
    
    std::forward_list<Register>& read_regs = m_reg_manager->getReadRegs();
    for (auto& read_reg : read_regs) {
        if (read_reg.name == name) {
            result = &read_reg;
            break;
        }
    }
    return result;
}

Register* DataManager::findDescribeRegOnlyByName(const std::string& name) {
    Register* result = nullptr;
    
    std::forward_list<Register>& read_regs = m_reg_manager->getDescribeRegs();
    for (auto& read_reg : read_regs) {
        if (read_reg.name == name) {
            result = &read_reg;
            break;
        }
    }
    return result;
}

Register* DataManager::findRegOnlyByName(const std::string& name) {
    Register* result = nullptr;    
    result = findReadRegOnlyByName(name);
    if (!result) result = findDescribeRegOnlyByName(name);
    return result;
}

Register* DataManager::findReadReg(const std::string& name, const int func, const int slave_id) {
    Register* result = nullptr;
    
    std::forward_list<Register>& read_regs = m_reg_manager->getReadRegs();
    for (auto& read_reg : read_regs) {
        if (read_reg.slave_id == slave_id && read_reg.function == func && read_reg.name == name) {
            result = &read_reg;
            break;
        }
    }
    return result;
}

Register* DataManager::findReadReg(const int addr, const int func, const int slave_id) {
    Register* result = nullptr;
    
    std::forward_list<Register>& read_regs = m_reg_manager->getReadRegs();
    for (auto& read_reg : read_regs) {
        if (read_reg.slave_id == slave_id && read_reg.function == func && read_reg.address == addr) {
            result = &read_reg;
            break;
        }
    }
    return result;
}

/** Создается если только адрес регистра входит в диапазон опроса из конфигурационного файла модбас */
Register* DataManager::createReadReg(const int address, const int func, const int slave_id) {
    Register* result = nullptr;

    const std::vector<Request>& read_requests = m_request_manager->getReadRequests();
    for (const auto& read_request : read_requests) {
        if (read_request.slave_id == slave_id && 
            read_request.function == func && read_request.isIncludeAddress(address)) {
            result = m_reg_manager->addReadReg(address, slave_id, read_request.function);
            break;
        }
    }
    
    return result;
}

uint8_t* DataManager::findU8DataMemory(const int addr, const int func, const int slave_id) {
    uint8_t* result = nullptr;
    uint8_t* local_start = nullptr;
    int addr_offset;

    const std::vector<Request>& read_requests = m_request_manager->getReadRequests();
    for (const auto& read_request : read_requests) {
        if (read_request.slave_id == slave_id && read_request.function == func && read_request.isIncludeAddress(addr)) {
            addr_offset = addr - read_request.address;
            local_start = read_request.mem_chunk->u8_ptr + read_request.offset_mem_chunk;
            result = local_start + addr_offset;
            break;
        }
    }
    
    return result;
}

uint16_t* DataManager::findU16DataMemory(const int addr, const int func, const int slave_id) {
    uint16_t* result = nullptr;
    uint16_t* local_start = nullptr;
    int addr_offset;

    const std::vector<Request>& read_requests = m_request_manager->getReadRequests();
    for (const auto& read_request : read_requests) {
        if (read_request.slave_id == slave_id && read_request.function == func && read_request.isIncludeAddress(addr)) {
            addr_offset = addr - read_request.address;
            local_start = read_request.mem_chunk->u16_ptr + read_request.offset_mem_chunk;
            result = local_start + addr_offset;
            break;
        }
    }
    
    return result;
}

const std::vector<Request>& DataManager::getReadRequests() { return m_request_manager->getReadRequests(); }

const int DataManager::getMaxCountReadRegs() { return m_request_manager->getMaxCountRegsRead(); }

const RegDataOrder DataManager::getDataOrder() { return m_reg_manager->getDataOrder(); }

// uint16_t DataManager::getCoil(uint16_t* ptr) { 
//     uint16_t val;
//     m_mem_manager->getWord(&val, ptr); 
//     return val;
// }

// float Data::getFloat16() { 
// 	float result;
// 	ModbusTrans::interpretReg16(result, *u16, reg->reg_info.data_type, reg->reg_info.precision);
// 	return result;
// }

// int Data::getShort() { 
// 	int result;

//     //  Data::getWord(uint16_t* val, int16_t* ptr) {

// 	ModbusTrans::interpretReg16(result, *u16, reg->reg_info.data_type);
// 	return result;
// }

// int Data::getInt() {
// 	int result = 0;
// 	uint16_t regs[2];
// 	regs[0] = *u16;
// 	regs[1] = *(u16+1);
// 	ModbusTrans::interpretReg32(result, regs, reg->reg_info.data_type, reg->reg_info.order);
// 	return result;
// }

// float Data::getFloat() {
// 	float result = 0;
// 	uint16_t regs[2];
// 	regs[0] = *u16;
// 	regs[1] = *(u16+1);
// 	ModbusTrans::interpretReg32(result, regs, reg->reg_info.data_type, reg->reg_info.order, reg->reg_info.precision);
// 	return result;
// }

} // data
} // mb


// считывать конфиг
/*
    считать заполнить ranges
    1. Read config
    2. Fill ranges
    3. Fill regs

    1. Сначала читаю ranges (создаю regs)
    2. Затем читаю регистры (правлю ranges, и одновременно создаю|правлю regs)
    3. Создаю запросы 
    4. Создаю память под запросы
*/
