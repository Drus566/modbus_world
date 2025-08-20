#include <iostream>

#include "Time.h"

#include "ModbusCoreRtu.h"

namespace mb {
namespace core {

ModbusCoreRtu::ModbusCoreRtu() {}

// Очистить байты
bool ModbusCoreRtu::flush() {
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
bool ModbusCoreRtu::receiveRequest(uint8_t* msg) {
	int rc;
	fd_set rset;
	unsigned int length_to_read; // Используется дял чтения заголовка
	int msg_length = 0;

	if (m_debug) {
		std::cout << "ModbusCoreRtu: Waiting for an indication..." << std::endl;
	}

	if (!m_serial.isConnected()) {
		std::cout << "ModbusCoreRtu: ERROR the connection is not established" << std::endl;
		return false;
	}

	/* Add a file descriptor to the set */
	FD_ZERO(&rset);
	FD_SET(m_serial.getSocket(), &rset);

	while (length_to_read != 0) {
		rc = m_serial.doSelect(&rset, m_indication_timeout);
		if (rc == -1) {
			if (m_error_recovery) {
				int saved_errno = errno;
				if (errno == ETIMEDOUT) {
					mb::helpers::sleep(m_connection.response_timeout);
					m_serial.flush();
				}
				else if (errno == EBADF) {
					m_serial.doClose();
					m_serial.connect();
				}
				errno = saved_errno;
			}
			return false;
		}

		rc = m_serial.receive(msg, length_to_read);
		if (rc == 0) {
			errno = ECONNRESET;
			rc == -1;
		}

		if (rc == -1) {
			std::cout << "ModbusCoreRtu: Error receiveRequest m_serial.receive method" << std::endl;
			if (m_error_recovery && (errno == ECONNRESET || errno == ECONNREFUSED || errno == EBADF)) {
				int saved_errno = errno;
				m_serial.doClose();
				m_serial.connect();
				errno = saved_errno;
			}
		}

		if (m_debug) {
			int i;
			for (i = 0; i < rc; i++) {
				printf("<%.2X>", msg[msg_length + i]);
			}
		}

		msg_length += rc;
		length_to_read -= rc;
	}
}

// Получить ответ|подтверждение, используется master
bool ModbusCoreRtu::receiveConfirmation() {

}

// Отправить запрос, используется master
bool ModbusCoreRtu::sendRequest() {

}

// Отправить ответ|подтверждение, используется master
bool ModbusCoreRtu::sendConfirmation() {

}

}
}