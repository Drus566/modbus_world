#ifndef MB_MODBUS_CORE_RTU_H
#define MB_MODBUS_CORE_RTU_H

#include <memory>

#include "IModbusCore.h"

namespace mb {
namespace core {

class ModbusCoreRtu : public IModbusCore {
public:
	ModbusCoreRtu();
	~ModbusCoreRtu();

	// Очистить байты
	bool flush() override;
	// Получить запрос, используется slave
	bool receiveRequest(uint8_t* msg) override;
	// Получить ответ|подтверждение, используется master
	bool receiveConfirmation() override;
	// Отправить запрос, используется master
	bool sendRequest() override;
	// Отправить ответ|подтверждение, используется master
	bool sendConfirmation() override;

private:
	mb::types::ModbusConnection m_connection;
	mb::interaction::Serial m_serial;

	// таймаут получения запроса слейвом в миллисекундах
	int m_indication_timeout;

	bool m_error_recovery;
	bool m_debug;
}; 

}
}

#endif // MB_MODBUS_CORE_RTU_H