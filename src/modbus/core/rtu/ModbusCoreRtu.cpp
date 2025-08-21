#include <iostream>

#include "ModbusCoreRtu.h"
#include "Time.h"

namespace mb {
namespace core {

ModbusCoreRtu::ModbusCoreRtu(mb::types::ModbusConnection con) {
	m_serial.init(con.rtu.serial_port, con.rtu.baudrate, con.rtu.parity, con.rtu.data_bits, con.rtu.stop_bit);
}

ModbusCoreRtu::~ModbusCoreRtu() {

}

// Создание
std::unique_ptr<IModbusCore> IModbusCore::create(mb::types::ModbusConnection connection) {
	return std::make_unique<ModbusCoreRtu>(connection);
}

// Коннект
bool ModbusCoreRtu::connect() {
	return m_serial.connect();
}

// Проверка соединения
bool ModbusCoreRtu::isConnect() {
	return m_serial.isConnected();
}
// Закрыть соединение
bool ModbusCoreRtu::close() {
	m_serial.doClose();
	return true;
}

// Очистить буфер
bool ModbusCoreRtu::clearBuffer() {
	bool result = m_serial.flush();
	if (result) {
		if (m_debug) std::cout << "ModbusCoreRtu: Bytes flushed" << std::endl;
	}
	else {
		if (m_debug) std::cout << "ModbusCoreRtu: Bytes flushed error" << std::endl;
	}
	return result;
}

// Получить запрос, используется slave
bool ModbusCoreRtu::getRequestFromMaster(uint8_t* msg) {
	int rc;
	fd_set rset;
	unsigned int length_to_read = 3; // Используется дял чтения заголовка

	while ((rc = m_serial.doSelect(&rset, m_indication_timeout)) >= 0) {
		rc = m_serial.receive(msg, length_to_read);
		if (rc == 0)
		{
			errno = ECONNRESET;
			rc == -1;
		}

		if (rc == -1)
		{
			std::cout << "ModbusCoreRtu: Error receiveRequest m_serial.receive method" << std::endl;
			if (m_try_reconnect && (errno == ECONNRESET || errno == ECONNREFUSED || errno == EBADF))
			{
				int saved_errno = errno;
				m_serial.doClose();
				m_serial.connect();
				errno = saved_errno;
			}
		}

		if (m_debug)
		{
			std::cout << "Receive message: " << msg << std::endl;
			std::cout << "Package receive: ";
			int i;
			for (i = 0; i < rc; i++)
			{
				printf("<%.2X>", msg[i]);
			}
			std::cout << std::endl;
		}
		mb::helpers::sleep(1000);
	}

	// int rc;
	// fd_set rset;
	// unsigned int length_to_read; // Используется дял чтения заголовка
	// int msg_length = 0;

	// if (m_debug) {
	// 	std::cout << "ModbusCoreRtu: Waiting for request from master..." << std::endl;
	// }

	// if (!m_serial.isConnected()) {
	// 	std::cout << "ModbusCoreRtu: ERROR the connection is not established" << std::endl;
	// 	return false;
	// }

	// /* Add a file descriptor to the set */
	// FD_ZERO(&rset);
	// FD_SET(m_serial.getSocket(), &rset);

	// while (length_to_read != 0) {
	// 	rc = m_serial.doSelect(&rset, m_indication_timeout);
	// 	if (rc == -1) {
	// 		if (m_try_reconnect) {
	// 			int saved_errno = errno;
	// 			if (errno == ETIMEDOUT) {
	// 				mb::helpers::sleep(m_connection.response_timeout);
	// 				m_serial.flush();
	// 			}
	// 			else if (errno == EBADF) {
	// 				m_serial.doClose();
	// 				m_serial.connect();
	// 			}
	// 			errno = saved_errno;
	// 		}
	// 		return false;
	// 	}

	// 	rc = m_serial.receive(msg, length_to_read);
	// 	if (rc == 0) {
	// 		errno = ECONNRESET;
	// 		rc == -1;
	// 	}

	// 	if (rc == -1) {
	// 		std::cout << "ModbusCoreRtu: Error receiveRequest m_serial.receive method" << std::endl;
	// 		if (m_try_reconnect && (errno == ECONNRESET || errno == ECONNREFUSED || errno == EBADF)) {
	// 			int saved_errno = errno;
	// 			m_serial.doClose();
	// 			m_serial.connect();
	// 			errno = saved_errno;
	// 		}
	// 	}

	// 	if (m_debug) {
	// 		// std::cout << "Package receive: ";
	// 		int i;
	// 		for (i = 0; i < rc; i++) {
	// 			printf("<%.2X>", msg[msg_length + i]);
	// 		}
	// 		// std::cout << std::endl;
	// 	}

	// 	msg_length += rc;
	// 	length_to_read -= rc;
	// }
}

// Получить ответ|подтверждение, используется master
bool ModbusCoreRtu::getResponseFromSlave() {

}

// Отправить запрос, используется master
bool ModbusCoreRtu::sendRequestToSlave() {
	uint8_t msg[] = { 'h', 'u', 'i'};
	long rc = m_serial.send(msg, 3);

	std::cout << "Send message '" << msg << "'" << std::endl; 

	if (m_debug) {
		std::cout << "Package send: ";
		int i;
		for (i = 0; i < rc; i++) {
			printf("<%.2X>", msg[i]);
		}
		std::cout << std::endl;
	}

	return true;
}

// Отправить ответ|подтверждение, используется master
bool ModbusCoreRtu::sendResponseToMaster() {

}

void ModbusCoreRtu::setDebug(bool flag) {
	m_debug = flag;
	m_serial.setDebug(flag);
}

}
}