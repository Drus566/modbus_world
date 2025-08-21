#ifndef MB_MODBUS_CORE_H
#define MB_MODBUS_CORE_H

#include "ModbusConnection.h"

namespace mb {
namespace core {

class IModbusCore {
public:
	virtual ~IModbusCore() {}

	// Создание
	static std::unique_ptr<IModbusCore> create(mb::types::ModbusConnection connection);
	// Коннект
	virtual bool connect() = 0;
	// Проверка соединения
	virtual bool isConnect() = 0;
	// Закрыть соединение
	virtual bool close() = 0;
	// Очистить байты
	virtual bool clearBuffer() = 0;
	// Получить запрос от мастера
	virtual bool getRequestFromMaster(uint8_t *msg) = 0;
	// Получить ответ от слейва
	virtual bool getResponseFromSlave() = 0;
	// Отправить запрос в слейв
	virtual bool sendRequestToSlave() = 0;
	// Отправить ответ в мастер
	virtual bool sendResponseToMaster() = 0;
	// Установить дебаг
	virtual void setDebug(bool flag) = 0;
}; 

}
}

#endif // MB_MODBUS_CORE_H