#ifndef MB_MODBUS_MASTER_H
#define MB_MODBUS_MASTER_H

#include <memory>

#include "IModbusCore.h"
#include "Serial.h"

// _MODBUS_TCP_HEADER_LENGTH 7
// _MODBUS_RTU_HEADER_LENGTH 1

namespace mb {
namespace core {

class ModbusMaster {
public:
	ModbusMaster(mb::types::ModbusConnection connection);
	~ModbusMaster();

	// Коннект
	bool connect() override;
	// Проверка соединения
	bool isConnect() override;
	// Закрыть соединение
	bool close() override;
	// Очистить байты
	bool clearBuffer() override;
	// Получить запрос, используется slave
	bool getRequestFromMaster(uint8_t *msg) override;
	// Получить ответ, используется master
	bool getResponseFromSlave() override;
	// Отправить запрос, используется master
	bool sendRequestToSlave() override;
	// Отправить ответ, используется master
	bool sendResponseToMaster() override;
	// Установка дебага
	void setDebug(bool flag) override;

private:
}; 

}
}

#endif // MB_MODBUS_MASTER_H