#include <iostream>

#include "ModbusRtu.h"
#include "Time.h"
#include "Crc.h"
#include "ModbusEnums.h"

#define RTU_READ_REQ_LENGTH 8
#define RTU_WRITE_SINGLE_REQ_LENGTH 8
#define MAX_RTU_MSG_LENGTH 252
#define CRC_SIZE 2

namespace mb {
namespace core {
	 
ModbusRtu::ModbusRtu(mb::types::ModbusConnection con) {
	m_serial.init(con.rtu.serial_port, con.rtu.baudrate, con.rtu.parity, con.rtu.data_bits, con.rtu.stop_bit);
}

ModbusRtu::~ModbusRtu() {

}

// Создание
std::unique_ptr<IModbusInteraction> IModbusInteraction::create(mb::types::ModbusConnection connection) {
	return std::make_unique<ModbusRtu>(connection);
}

// Коннект
bool ModbusRtu::connect() {
	return m_serial.connect();
}

// Проверка соединения
bool ModbusRtu::isConnect() {
	return m_serial.isConnected();
}
// Закрыть соединение
bool ModbusRtu::close() {
	m_serial.doClose();
	return true;
}

// Очистить буфер
bool ModbusRtu::clearBuffer() {
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
bool ModbusRtu::getRequestFromMaster(uint8_t* msg) {
	// int rc;
	// fd_set rset;
	// unsigned int length_to_read = 3; // Используется дял чтения заголовка

	// while ((rc = m_serial.doSelect(&rset, m_indication_timeout)) >= 0) {
	// 	rc = m_serial.receive(msg, length_to_read);
	// 	if (rc == 0)
	// 	{
	// 		errno = ECONNRESET;
	// 		rc == -1;
	// 	}

	// 	if (rc == -1)
	// 	{
	// 		std::cout << "ModbusCoreRtu: Error receiveRequest m_serial.receive method" << std::endl;
	// 		if (m_try_reconnect && (errno == ECONNRESET || errno == ECONNREFUSED || errno == EBADF))
	// 		{
	// 			int saved_errno = errno;
	// 			m_serial.doClose();
	// 			m_serial.connect();
	// 			errno = saved_errno;
	// 		}
	// 	}

	// 	if (m_debug)
	// 	{
	// 		std::cout << "Receive message: " << msg << std::endl;
	// 		std::cout << "Package receive: ";
	// 		int i;
	// 		for (i = 0; i < rc; i++)
	// 		{
	// 			printf("<%.2X>", msg[i]);
	// 		}
	// 		std::cout << std::endl;
	// 	}
	// 	mb::helpers::sleep(1000);
	// }

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
bool ModbusRtu::getResponseFromSlave() {

}

// Отправить запрос, используется master
bool ModbusRtu::sendRequestToSlave(uint8_t* msg, int length) {
	return true;
}

// Отправить запрос, используется master
bool ModbusRtu::sendReadReq(mb::core::Data& data) {
	uint8_t msg[RTU_READ_REQ_LENGTH];
	// Slave id
	msg[0] = data.id;
	// Func
	msg[1] = data.func;
	// Start adr
	msg[2] = (data.payload_1 >> 8) & 0xFF; // Старший байт
	msg[3] = data.payload_1 & 0x00FF;		// Младший байт
	// Quantity
	msg[4] = (data.payload_2 >> 8) & 0xFF; // Старший байт
	msg[5] = data.payload_2 & 0x00FF;		// Младший байт
	
	uint16_t crc = mb::helpers::crc16(msg,6);
	// CRC
	msg[6] = crc & 0x00FF;						// Младший байт
	msg[7] = (crc >> 8) & 0xFF;				// Старший байт

	return m_serial.send(msg, RTU_READ_REQ_LENGTH);
}

bool ModbusRtu::sendWriteSingleReq(mb::core::Data &data) {
	uint8_t msg[RTU_WRITE_SINGLE_REQ_LENGTH];
	// Slave id
	msg[0] = data.id;
	// Func
	msg[1] = data.func;
	// Write adr
	msg[2] = (data.payload_1 >> 8) & 0xFF; // Старший байт
	msg[3] = data.payload_1 & 0x00FF;		// Младший байт
	// Write val
	msg[4] = (data.payload_2 >> 8) & 0xFF; // Старший байт
	msg[5] = data.payload_2 & 0x00FF;		// Младший байт
	// Crc
	uint16_t crc = mb::helpers::crc16(msg, 6);
	msg[6] = crc & 0x00FF;		 // Младший байт
	msg[7] = (crc >> 8) & 0xFF; // Старший байт

	return m_serial.send(msg, RTU_WRITE_SINGLE_REQ_LENGTH);
}

bool ModbusRtu::sendWriteMultipleReq(mb::core::Data &data) {
	uint8_t msg[MAX_RTU_MSG_LENGTH];
	// Slave id
	msg[0] = data.id;
	// Func
	msg[1] = data.func;
	// Start adr
	msg[2] = (data.payload_1 >> 8) & 0xFF; // Старший байт
	msg[3] = data.payload_1 & 0x00FF;		// Младший байт
	// Quantity
	msg[4] = (data.payload_2 >> 8) & 0xFF; // Старший байт
	msg[5] = data.payload_2 & 0x00FF;		// Младший байт
	// Byte count
	msg[6] = data.byte_count;

	if (data.func == mb::types::FuncNumber::WRITE_MULTIPLE_COILS) {
		uint8_t* bit_val_ptr = msg + 7; 	// Указатель на начальный байт для записи битов
		uint8_t data_bit_val = 0;			// Значение бита для записи
		uint8_t bit_number = 0;			  	// Номер бита в байте
		uint8_t result_byte = 0;			// Для сохранения результата в памяти
		bool is_new_byte = false;

		for (int i = 0; i < data.payload_2; i++) {
			data_bit_val = *(data.bit_vals + i);

			// Если бит 1
			if (data_bit_val) result_byte |= (1u << bit_number); // Установка 1
			// Если бит 0
			else result_byte &= ~(1u << bit_number); // Установка 0

			++bit_number;

			// Каждый новый байт сохраняем результат и обнуляем байт и номер бита
			is_new_byte = (bit_number & 7) == 0;
			if (is_new_byte) { 
				*bit_val_ptr++ = result_byte;
				bit_number = 0;
				result_byte = 0;
			}
		}
		// Если не было нового байта, сохраняем результат
		if (!is_new_byte) *bit_val_ptr = result_byte;
	}
	else if (data.func == mb::types::FuncNumber::WRITE_MULTIPLE_WORDS) {
		uint8_t* word_val_ptr = msg + 7; // Указатель на начальное слово для записи
		uint16_t data_word_val = 0;		// Слово для записи

		for (int i = 0; i < data.payload_2; i++) {
			data_word_val = *(data.words_val + i);
			*word_val_ptr++ = (data_word_val >> 8) & 0xFF;	// Старший байт
			*word_val_ptr++ = data_word_val & 0x00FF; 		// Младший байт
		}
	}

	// Crc
	uint8_t crc_pos = 7 + data.byte_count;
	uint16_t crc = mb::helpers::crc16(msg, crc_pos);
	msg[crc_pos] = crc & 0x00FF;			  // Младший байт
	msg[crc_pos + 1] = (crc >> 8) & 0xFF; // Старший байт

	return m_serial.send(msg, RTU_WRITE_SINGLE_REQ_LENGTH);
}

// inline WORD helperWriteWordBit(WORD word, BIT bit_number, BIT bit_val)
// {
// 	WORD result;
// 	if (bit_val)
// 		result = word | (1 << bit_number); // Установка 1
// 	else
// 		result = word & ~(1 << bit_number); // Установка 0
// 	return result;
// }

// inline BIT helperReadWordBit(WORD word, BIT bit_number)
// {
// 	return (word >> bit_number) & 1;
// }

// inline WORD helperInvertWordBit(WORD word, BIT bit_number)
// {
// 	return word = word ^ (1 << bit_number);
// }

// Отправить ответ|подтверждение, используется master
bool ModbusRtu::sendResponseToMaster(uint8_t* msg, int length) {

}

void ModbusRtu::setDebug(bool flag) {
	m_debug = flag;
	m_serial.setDebug(flag);
}

}
}