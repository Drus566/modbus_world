#ifndef MB_MODBUS_CORE_RTU_H
#define MB_MODBUS_CORE_RTU_H

#include <memory>

#include "IModbusCore.h"
#include "Serial.h"

// _MODBUS_TCP_HEADER_LENGTH 7
// _MODBUS_RTU_HEADER_LENGTH 1

namespace mb {
namespace core {

class ModbusCoreRtu : public IModbusCore {
public:
	ModbusCoreRtu(mb::types::ModbusConnection connection);
	~ModbusCoreRtu();

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
	mb::types::ModbusConnection m_connection;
	mb::interaction::Serial m_serial;
	// таймаут получения запроса слейвом в миллисекундах
	int m_indication_timeout;
	// попытка восстановление соединения после ошибок
	bool m_try_reconnect;
	bool m_debug;
}; 

}
}

#endif // MB_MODBUS_CORE_RTU_H