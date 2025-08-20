#ifndef MB_DATA_MANAGER_H
#define MB_DATA_MANAGER_H

#include "Config.h"
#include "MemManager.h"
#include "ModbusEnums.h"
#include "ModbusTrans.h"
#include "ModbusRegister.h"
#include "ModbusRequest.h"
#include "Logger.h"

#include <string>

namespace mb {
namespace data {

using namespace mb::config;
using namespace mb::types;
using namespace mb::mem;
using namespace mb::log;

class RegManager;
class RangeManager;
class RequestManager;

class DataManager {    
public:
    DataManager(Config* config, MemManager* mem_manager);
    ~DataManager();
    
    bool start();

    Register* findRegOnlyByName(const std::string& name);
    Register* findDescribeRegOnlyByName(const std::string& name);
    Register* findReadRegOnlyByName(const std::string& name);
    Register* findReadReg(const std::string& name, const int func, const int slave_id);
    Register* findReadReg(const int addr, const int func, const int slave_id);
    Register* createReadReg(const int address, const int func, const int slave_id);
    uint8_t*  findU8DataMemory(const int addr, const int func, const int slave_id);
    uint16_t* findU16DataMemory(const int addr, const int func, const int slave_id);

    const std::vector<Request>& getReadRequests();
    const int getMaxCountReadRegs();
    const RegDataOrder getDataOrder();

    void printRegs();

private:
    bool parseRanges(int slave_id, SectionsMap& map);
    bool parseRegs(int slave_id, SectionsMap& map);
    void completeRanges();
    void createRequests();

    Config* m_config;
    MemManager* m_mem_manager;

    RangeManager* m_range_manager;
    RegManager* m_reg_manager;
    RequestManager* m_request_manager;

    bool m_print_settings;
    bool m_print_regs;
};

} // data
} // mb

#endif // MB_DATA_MANAGER_H

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
