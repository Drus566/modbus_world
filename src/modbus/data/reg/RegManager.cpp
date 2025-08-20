#include "RegManager.h"
#include "Logger.h"

#include <iostream>
#include <sstream>
#include <string>

namespace mb {
namespace data {

using namespace mb::log;

RegManager::RegManager(RegDataOrder data_order) : m_data_order(data_order) {}

RegDataOrder RegManager::getDataOrder() { return m_data_order; }

bool RegManager::parseReg(const bool is_describe, const std::string& reg_str, int& address, RegisterInfo& reg_info) {
	bool result = false;
	char delimeter = ',';
	char precision_delimeter = '_';

	std::vector<std::string> tokens = split(reg_str, delimeter);
	// is word
	if (tokens.size() > 1) {
		for (auto it = tokens.begin(); it != tokens.end(); ++it) {
			*it = trim(*it);
			*it = toUpperCase(*it);

			// parse address
			if (isNumber(*it)) {
				address = std::stoi(*it);
				result = true;
			}
			else {
				// parse data_type
				if (getRegDataTypeFromStr(reg_info.data_type, *it) && isFloatDataType(reg_info.data_type)) {
					std::vector<std::string> float_tokens = split(*it, precision_delimeter);
					if (float_tokens.size() > 1) {
						std::string precision_str = float_tokens.at(1);
						if (isNumber(precision_str)) reg_info.precision = std::stoi(precision_str);
					}
				}
				// parse data_order
				else getRegDataOrderFromStr(reg_info.order, *it);
			}
		}
	}
	// is coil
	else if (isNumber(reg_str)) {
		address = std::stoi(reg_str);
		result = true;			
	}
	return result;
}

bool RegManager::addReg(const bool is_describe, const int slave_id, const FuncNumber func, const std::string& name, std::string& reg_str) {
	bool result = false;
	
	char delimeter = ',';
	char precision_delimeter = '_';

	int address;
	int val = 0;
	reg_str = trim(reg_str);

	RegisterInfo reg_info;

	// parse func
	result = parseReg(is_describe, reg_str, address, reg_info);

	if (isDwordDataType(reg_info.data_type) && reg_info.order == RegDataOrder::NONE) reg_info.order = m_data_order;
	
	if (result) { 
		if (is_describe) m_describe_regs.emplace_front(address, reg_info, name, slave_id, func); 
		else if (isReadFunc(func)) m_regs.emplace_front(address, reg_info, name, slave_id, func);
	}

	return result;
}

Register* RegManager::addReadReg(const int address, const int slave_id, const FuncNumber func) {
	RegisterInfo reg_info; 
	m_regs.emplace_front(address, reg_info, "", slave_id, func);
	auto it = m_regs.begin();
	Register* reg = &(*it);
	return reg;
}

std::forward_list<Register>& RegManager::getReadRegs() { return m_regs; }

std::forward_list<Register> &RegManager::getDescribeRegs() { return m_describe_regs; }

void RegManager::printInfo() {
	Logger::Instance()->rawLog("** REGISTERS [name, slave_id, func, addr, data_type, order, precision] **");
	Logger::Instance()->rawLog(" READ REGISTERS ");
	for (const Register& r : m_regs) {
		printRegInfo(r);
	}
	Logger::Instance()->rawLog(" DESCRIBE REGISTERS ");
	for (const Register& r : m_describe_regs) {
		printRegInfo(r);
	}
	Logger::Instance()->rawLog("*************************************************************************");
}

void RegManager::printRegInfo(const Register& r) {
	std::ostringstream oss;
	oss << "[" << r.name << "," << r.slave_id << ","
		 << r.function << "," << r.address;

	if (r.isWord()) {
		oss << "," << RegDataTypeToString(r.reg_info.data_type);
		if (r.isDword()) oss << "," << RegDataOrderToString(r.reg_info.order);
		if (r.isFloat()) oss << "," << static_cast<int>(r.reg_info.precision);
	}
	oss << "]";
	Logger::Instance()->rawLog("%s", oss.str().c_str());
}

} // data
} // mb