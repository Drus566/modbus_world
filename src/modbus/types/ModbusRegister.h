#ifndef MB_MODBUS_REGISTER_H
#define MB_MODBUS_REGISTER_H

#include "String.h"
#include "ModbusEnums.h"

namespace mb {
namespace types {

struct RegisterInfo {
    RegDataType data_type;  // тип данных регистра
    RegDataOrder order;     // порядок следования байт
    char precision;         // точность
    
    RegisterInfo(RegDataType data_type = RegDataType::INT16, 
                 RegDataOrder order = RegDataOrder::NONE, 
                 char precision = 1)
        : data_type(data_type), order(order), precision(precision) {}
};

struct Register {
    /**** Дополнительные поля для регистров с функцией записи */
    // bool is_write;            // тип регистра - запись
    // unsigned char coil_write; // значение записи койла
    // RegisterValue* reg_val;   // для функций записи

    int address;              // адрес регистра
    int slave_id;             // адрес модбас
    FuncNumber function;      // функция
    std::string name;         // имя регистра
    RegisterInfo reg_info;    // доп инфа о регистре (если это слово)

    Register(int addr, RegisterInfo info, std::string name, int id = 1, FuncNumber func = FuncNumber::READ_REGS)
        : address(addr), reg_info(info), name(name), slave_id(id), function(func) {}
    
    bool isCoil() const { return function == FuncNumber::READ_COIL || function == FuncNumber::READ_INPUT_COIL; }
    bool isWord() const { return !isCoil(); }
    bool isDword() const { return isWord() && (reg_info.data_type == RegDataType::FLOAT32 || reg_info.data_type == RegDataType::INT32 || reg_info.data_type == RegDataType:: UINT32); }
    bool isFloat() const { return isWord() && (reg_info.data_type == RegDataType::FLOAT16 || reg_info.data_type == RegDataType::FLOAT32); }
    // bool isWrite() const { return is_write; }
};

} // types
} // mb

#endif // MB_MODBUS_REGISTER_H