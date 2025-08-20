#ifndef MB_REG_MANAGER_H
#define MB_REG_MANAGER_H

#include "ModbusEnums.h"
#include "ModbusTrans.h"
#include "ModbusRegister.h"

#include <forward_list>
#include <string>

namespace mb {
namespace data {
    
using namespace mb::types;

constexpr RegDataOrder DEFAULT_DATA_ORDER = RegDataOrder::CD_AB;

class RegManager {    
    public:
        RegManager(RegDataOrder data_order);

        RegDataOrder getDataOrder();
        std::forward_list<Register>& getReadRegs();
        std::forward_list<Register>& getDescribeRegs();
        bool addReg(bool is_describe, const int slave_id, const FuncNumber func, const std::string& name, std::string& reg_str);
        Register* addReadReg(const int address, const int slave_id, const FuncNumber func);

        void printInfo();

    private:
        RegDataOrder m_data_order;

        std::forward_list<Register> m_regs;
        std::forward_list<Register> m_describe_regs;

        bool parseReg(const bool is_describe, const std::string &reg_str, int &address, RegisterInfo &reg_info);

        void printRegInfo(const Register &r);
};

} // data
} // mb

#endif // MB_REG_MANAGER_H