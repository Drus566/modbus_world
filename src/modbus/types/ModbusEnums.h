
#ifndef MB_MODBUS_ENUMS_H
#define MB_MODBUS_ENUMS_H

#include "String.h"

namespace mb {
namespace types {

using namespace mb::helpers;

/** @brief типы данных регистров */
enum class RegDataType {
    UINT16,
    INT16,
    FLOAT16,
    FLOAT32,
    INT32,
    UINT32
};

// преобразование enum в строку
inline const char* RegDataTypeToString(RegDataType e) noexcept {
    switch(e) {
        case RegDataType::UINT16:  return "UINT16";
        case RegDataType::INT16:   return "INT16";
        case RegDataType::FLOAT16: return "FLOAT16";
        case RegDataType::FLOAT32: return "FLOAT32";
        case RegDataType::INT32:   return "INT32";
        case RegDataType::UINT32:  return "UINT32";
    }
    return "";
};

inline bool isFloatDataType(const RegDataType& type) { return (type == RegDataType::FLOAT16 || type == RegDataType::FLOAT32); }

inline bool isDwordDataType(const RegDataType& type) { return (type == RegDataType::INT32 || type == RegDataType::UINT32 || type == RegDataType::FLOAT32); }

// Функция для проверки соответствия строки значению enum
inline bool getRegDataTypeFromStr(RegDataType& data_type, const std::string& str) {
    bool result = false;
    for (int i = static_cast<int>(RegDataType::UINT16); i <= static_cast<int>(RegDataType::UINT32); ++i) {
        RegDataType type = static_cast<RegDataType>(i);
        if (startsWith(str, RegDataTypeToString(type))) {
            data_type = type;
            result = true;
            break;
        }
    }
    return result;
}

/** @brief расположение регистров в памяти для 32/64 разрядных значений */
enum class RegDataOrder {
    // отсутствует
    NONE, 
    // для 32 битных
    AB_CD,
    CD_AB,
    BA_DC,
    DC_BA,
    // для 64 битных
    AB_CD_EF_GH,
    GH_EF_CD_AB,
    BA_DC_FE_HG,
    HG_FE_DC_BA
};

// преобразование enum в строку
inline const char* RegDataOrderToString(RegDataOrder e) noexcept {
    switch(e) {
        case RegDataOrder::AB_CD:        return "AB_CD";
        case RegDataOrder::CD_AB:        return "CD_AB";
        case RegDataOrder::BA_DC:        return "BA_DC";
        case RegDataOrder::DC_BA:        return "DC_BA";
        case RegDataOrder::AB_CD_EF_GH:  return "AB_CD_EF_GH";
        case RegDataOrder::GH_EF_CD_AB:  return "GH_EF_CD_AB";
        case RegDataOrder::BA_DC_FE_HG:  return "BA_DC_FE_HG";
        case RegDataOrder::HG_FE_DC_BA:  return "HG_FE_DC_BA";
    }
    return "";
};

// Функция для проверки соответствия строки значению enum
inline bool getRegDataOrderFromStr(RegDataOrder& data_order, const std::string& str) {
    bool result = false;
    for (int i = static_cast<int>(RegDataOrder::AB_CD); i <= static_cast<int>(RegDataOrder::HG_FE_DC_BA); ++i) {
        RegDataOrder type = static_cast<RegDataOrder>(i);
        if (startsWith(str, RegDataOrderToString(type))) {
            result = true;
            data_order = type;
            break;
        }
    }
    return result;
}


/** @brief номер функции */
enum FuncNumber {
    // чтение койлов
    READ_COIL = 1,
    // чтение койлов из участка памяти input
    READ_INPUT_COIL = 2,
    // чтение регистров
    READ_REGS = 3,
    // чтение регистров из участка памяти input
    READ_INPUT_REGS = 4,
    // запись одного койла
    WRITE_SINGLE_COIL = 5,
    // запись одного слова
    WRITE_SINGLE_WORD = 6,
    // запись множества койлов
    WRITE_MULTIPLE_COILS = 15,
    // запись множества слов
    WRITE_MULTIPLE_WORDS = 16
};

inline bool isReadWordFunc(const FuncNumber& func) {
    return func == FuncNumber::READ_REGS || func == FuncNumber::READ_INPUT_REGS;
}

inline bool isReadCoilFunc(const FuncNumber& func) {
    return func == FuncNumber::READ_COIL || func == FuncNumber::READ_INPUT_COIL;
}

inline bool isReadFunc(const FuncNumber& func) { 
    return (func == FuncNumber::READ_COIL || func == FuncNumber::READ_INPUT_COIL || 
            func == FuncNumber::READ_REGS || func == FuncNumber::READ_INPUT_REGS);
}

inline bool isCoilFunc(const FuncNumber& func) {
    return (func == FuncNumber::READ_COIL || func == FuncNumber::READ_INPUT_COIL ||
            func == FuncNumber::WRITE_SINGLE_COIL || func == FuncNumber::WRITE_MULTIPLE_COILS);
}

inline bool isWriteMultipleFunc(const FuncNumber& func) { return (func == FuncNumber::WRITE_MULTIPLE_COILS || func == FuncNumber::WRITE_MULTIPLE_WORDS); }


inline bool isWriteWordOrWordsFunc(const FuncNumber& func) { 
    return (func == FuncNumber::WRITE_SINGLE_WORD || func == FuncNumber::WRITE_MULTIPLE_WORDS);
}

inline bool isWriteCoilOrCoilsFunc(const FuncNumber& func) {
    return (func == FuncNumber::WRITE_SINGLE_COIL || func == FuncNumber::WRITE_MULTIPLE_COILS);
}




inline bool isReadWordFunc(const int func) {
    return func == FuncNumber::READ_REGS || func == FuncNumber::READ_INPUT_REGS;
}

inline bool isReadCoilFunc(const int func) {
    return func == FuncNumber::READ_COIL || func == FuncNumber::READ_INPUT_COIL;
}

inline bool isReadFunc(const int func) { 
    return (func == FuncNumber::READ_COIL || func == FuncNumber::READ_INPUT_COIL || 
            func == FuncNumber::READ_REGS || func == FuncNumber::READ_INPUT_REGS);
}

inline bool isCoilFunc(const int func) {
    return (func == FuncNumber::READ_COIL || func == FuncNumber::READ_INPUT_COIL ||
            func == FuncNumber::WRITE_SINGLE_COIL || func == FuncNumber::WRITE_MULTIPLE_COILS);
}

inline bool isWriteMultipleFunc(const int func) { return (func == FuncNumber::WRITE_MULTIPLE_COILS || func == FuncNumber::WRITE_MULTIPLE_WORDS); }


inline bool isWriteWordOrWordsFunc(const int func) { 
    return (func == FuncNumber::WRITE_SINGLE_WORD || func == FuncNumber::WRITE_MULTIPLE_WORDS);
}

inline bool isWriteCoilOrCoilsFunc(const int func) {
    return (func == FuncNumber::WRITE_SINGLE_COIL || func == FuncNumber::WRITE_MULTIPLE_COILS);
}

} // types
} // mb

#endif // MB_MODBUS_ENUMS_H

