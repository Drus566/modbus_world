#ifndef MB_MODBUS_ETH_H
#define MB_MODBUS_ETH_H

#include "IModbusInteraction.h"

namespace mb {
namespace core {

class ModbusEth  {
public:
	ModbusEth();
	~ModbusEth();

	// Очистить байты
	// bool flush() override;
	// // Получить запрос, используется slave
	// bool receiveRequest() override;
	// // Получить ответ|подтверждение, используется master
	// bool receiveConfirmation() override;
	// // Отправить запрос, используется master
	// bool sendRequest() override;
	// // Отправить ответ|подтверждение, используется master
	// bool sendConfirmation() override;

private:
	mb::types::ModbusConnection m_connection;

	bool m_error_recovery;
	bool m_debug;
}; 

}
}

#endif // MB_MODBUS_ETH_H