#ifndef MB_MODBUS_CORE_H
#define MB_MODBUS_CORE_H

#include <memory>

namespace mb {
namespace core {

class ModbusCore {
public:

private:
	std::unique_ptr<IModbusInteraction> interaction;
}; 

}
}

#endif // MB_MODBUS_CORE_H