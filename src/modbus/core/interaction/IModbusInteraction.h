#ifndef MB_MODBUS_INTERACTION_H
#define MB_MODBUS_INTERACTION_H

#include "ModbusConnection.h"

namespace mb {
namespace core {

// Данные для формирования пакета запроса
struct Data {
	uint8_t *bit_vals;   // Указать на битовые данные, используется при записи множества битов
	uint16_t *words_val; // Указатель на слова, используется при записи множества слов
	uint16_t payload_1;  // Может являться Start adr, Out Adr, Reg Adr
	uint16_t payload_2;  // Может являться Quantity, Out val, Reg val
	uint8_t id;				// ID мастера или слейва
	uint8_t func;			// Номер фукнции модбас
	uint8_t byte_count;	// Количество байт, используется при записи множества слов/байт
};

class IModbusInteraction {
public:
	virtual ~IModbusInteraction() {}

	// Создание
	static std::unique_ptr<IModbusInteraction> create(mb::types::ModbusConnection connection);
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
	virtual bool sendRequestToSlave(uint8_t *msg, int length) = 0;
	// Отправить ответ в мастер
	virtual bool sendResponseToMaster(Data &data) = 0;

	// Отправить запрос слейву
	virtual bool sendRequest(Data &data) = 0;
	// Получить ответ на запрос от слейва
	virtual bool getResponse(Data &data, int length) = 0;

	// Установить дебаг
	virtual void setDebug(bool flag) = 0;
}; 

}
}

#endif // MB_MODBUS_INTERACTION_H