#ifndef MB_MODBUS_RTU_H
#define MB_MODBUS_RTU_H

#include <memory>

#include "IModbusInteraction.h"
#include "Serial.h"

// _MODBUS_TCP_HEADER_LENGTH 7
// _MODBUS_RTU_HEADER_LENGTH 1

namespace mb {
namespace core {

class ModbusRtu : public IModbusInteraction {
public:
	ModbusRtu(mb::types::ModbusConnection connection);
	~ModbusRtu();

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
	bool sendRequestToSlave(uint8_t *msg, int length) override;
	// Отправить ответ, используется master
	bool sendResponseToMaster(uint8_t *msg, int length) override;

	// Отправить запрос чтения
	bool sendReadReq(mb::core::Data& data) override;
	// Отправить запрос записи одного койла/регистра
	bool sendWriteSingleReq(mb::core::Data& data) override;
	// Отправить запрос записи множества койлов/регистров
	bool sendWriteMultipleReq(mb::core::Data& data) override;

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

#endif // MB_MODBUS_RTU_H