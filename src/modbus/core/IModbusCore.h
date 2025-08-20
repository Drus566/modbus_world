#ifndef MB_MODBUS_CORE_H
#define MB_MODBUS_CORE_H

#include "Serial.h"
#include "ModbusConnection.h"

namespace mb {
namespace core {

class IModbusCore {
public:
	// Очистить байты
	virtual bool flush() = 0;
	// Получить запрос, используется slave
	virtual bool receiveRequest(uint8_t* msg) = 0;
	// Получить ответ|подтверждение, используется master
	virtual bool receiveConfirmation() = 0;
	// Отправить запрос, используется master
	virtual bool sendRequest() = 0;
	// Отправить ответ|подтверждение, используется master
	virtual bool sendConfirmation() = 0;
}; 

}
}

#endif // MB_MODBUS_CORE_H