#ifndef MB_MODBUS_CORE_ETH_H
#define MB_MODBUS_CORE_ETH_H

#include "IModbusCore.h"

namespace mb {
namespace core {

class ModbusCoreEth : public IModbusCore {
public:
	ModbusCoreEth();
	~ModbusCoreEth();
	
	// Очистить байты
	bool flush() override;
	// Получить запрос, используется slave
	bool receiveRequest() override;
	// Получить ответ|подтверждение, используется master
	bool receiveConfirmation() override;
	// Отправить запрос, используется master
	bool sendRequest() override;
	// Отправить ответ|подтверждение, используется master
	bool sendConfirmation() override;

private:
	mb::types::ModbusConnection m_connection;

	bool m_error_recovery;
	bool m_debug;
}; 

}
}

#endif // MB_MODBUS_CORE_ETH_H