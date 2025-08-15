#ifndef MB_LINGUIST_H
#define MB_LINGUIST_H

#include <vector>
#include <cstdint> 
#include <string>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <mutex>

namespace mb {
namespace modbus {

// Задаваемые пользователем (user-defined function codes) - 65...72, 100...110. Эти коды не описаны в спецификации стандарта и могут использоваться в конкретных изделиях для собственных функций.
// Зарезервированные (reserved). В эту группу входят коды 9, 10, 13, 14, 41, 42, 90, 91, 125, 126 и 127.

/*
0x01 (1) - чтение значений из нескольких регистров флагов - Read Coil Status.
0x02 (2) - чтение значений из нескольких дискретных входов - Read Discrete Inputs.
0x03 (3) - чтение значений из нескольких регистров хранения - Read Holding Registers.
0x04 (4) - чтение значений из нескольких регистров ввода - Read Input Registers.
0x05 (5) - запись значения одного флага - Force Single Coil.
0x06 (6) - запись значения в один регистр хранения - Preset Single Register.
0x07 (7) - чтение сигналов состояния - Read Exception Status.
0x08 (8) - диагностика - Diagnostic.
0x0B (11) - чтение счетчика событий - Get Com Event Counter.
0x0C (12) - чтение журнала событий - Get Com Event Log.
0x0F (15) - запись значений в несколько регистров флагов - Force Multiple Coils.
0x10 (16) - запись значений в несколько регистров хранения - Preset Multiple Registers.
0x11 (17) - чтение информации об устройстве - Report Slave ID.
0x14 (20) - чтение из файла - Read File Record.
0x15 (21) - запись в файл - Write File Record.
0x16 (22) - запись в один регистр хранения с использованием маски "И" и маски "ИЛИ" - Mask Write Register.
0x18 (24) - чтение данных из очереди - Read FIFO Queue.
0x2B (43) - Encapsulated Interface Transport.
*/

enum class ModbusFunc {
    F_01 = 1, // Read Coil Status
    F_02 = 2, // Read Input Status
    F_03 = 3, // Read Holding Registers
    F_04 = 4, // Read Input Registers
    F_05 = 5, // Write Single Coil
    F_06 = 6, // Write Single Register
    F_15 = 15, // Write Multiple Coils
    F_16 = 16, // Write Multiple Registers
};

enum class ModbusFuncError {
    FErr_01 = 129, // Error Read Coil Status
    FErr_02 = 130, // Error Read Input Status
    FErr_03 = 131, // Error Read Holding Registers
    FErr_04 = 132, // Error Read Input Registers
    FErr_05 = 133, // Error Write Single Coil
    FErr_06 = 134, // Error Write Single Register
    FErr_15 = 143, // Error Write Multiple Coils
    FErr_16 = 144, // Error Write Multiple Registers
};

enum class ModbusRTU_Read {
    SLAVE_POS = 0,
    FUNC_POS = 1,
    START_ADR_POS = 2,
    START_ADR_SIZE = 2,
    QUANTITY_POS = 4,
    QUANTITY_SIZE = 2,
    CRC_POS
};

#define WORD_BIT_SIZE 16
#define DWORD_BIT_SIZE 32

#define DWORD  uint32_t
#define WORD   uint16_t
#define BYTE   uint8_t
#define BIT    uint8_t

/** @brief типы данных карт */
enum class MapType {
	BIT_MAP,
	WORD_MAP,
};

/** @brief тип хранения в памяти */
enum class MemMode {
	EMPTY,
	BIG_ENDIAN_MODE,
	LITTLE_ENDIAN_MODE,
	BIG_ENDIAN_BYTE_SWAP_MODE,	   // Byte Swap каждого 16-битного слова в 32-битном числе
	LITTLE_ENDIAN_BYTE_SWAP_MODE, // Byte Swap каждого 16-битного слова в 32-битном числе
};

constexpr MemMode default_mem_mode = MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE;

/** @brief Класс отвечает за создание(привязки) карты памяти последовательных адресов.
	Карта памяти может быть в виде битов (BIT) или слов (WORD) и имеет начальный адрес и количество регистров (слов или битов)
	Предоставляет функции чтение и записи в карту памяти 
*/

// Пример. Карта памяти, начальный адрес 231, количество регистров 8
// Адрес | Данные(16 бит, регистр)
// -- -- | -- -- -- -- -- -- -- --
// (231) | 16162
// (233) | 7325
// (235) | 255
// (237) | 3
// (239) | 25
// (241) | 11
// (243) | 5401
// (245) | 19084

// 1,2,3,4 функции запрос
struct RTU_READ_REQ {
	WORD start_adr;
	WORD quantity;
	WORD crc;
};

// 5,6 функции
struct RTU_WRITE_REQ {
	WORD adr;
	WORD val;
	WORD crc;
};

// 15,16 функции
struct RTU_WRITES_REQ {
	union {
		BYTE* byte_vals;
		WORD* word_vals;
	};
	WORD start_adr;
	WORD quantity;
	WORD crc;
	BYTE byte_count;
};

// 1,2,3,4 функции ответ
struct RTU_READ_RESP {
	union {
		BYTE* byte_vals;
		WORD* word_vals;
	};
	WORD crc;
	BYTE count;
};

// 5,6 функции ответ
struct RTU_WRITE_RESP {
	WORD adr;
	WORD val;
	WORD crc;
};

// 15,16 функции ответ
struct RTU_WRITES_RESP {
	WORD start_adr;
	WORD quantity;
	WORD crc;
};

// Ответ ошибкой, подходит ко всем функциям
struct RTU_ERROR_RESP {
	WORD crc;
	BYTE exception_code;
};

union RTU_PACKAGE {
	BYTE slave;
	BYTE func;

	RTU_READ_REQ read_req;
	RTU_WRITE_REQ write_req;
	RTU_WRITES_REQ writes_req;
	RTU_READ_RESP read_resp;
	RTU_WRITE_RESP write_resp;
	RTU_WRITES_RESP writes_resp;
	RTU_ERROR_RESP error_resp;
};



class ModbusLinguist {
public:
	ModbusLinguist();

	RTU_READ_RESP parseRTUReadResponse(char* package);

	void parseRTUReqPackage(char* package) {
		BYTE slave = *package;
		BYTE func = *(package + 1);
		switch (func) {
			case 1:
			case 2:
			case 3:
			case 4:
				WORD start_adr = *(package + 2);
				WORD quantity = *(package + 4);
				WORD crc = *(package + 6); 
				break;
			
			case 5:
			case 6:
				WORD adr = *(package + 2);
				WORD val = *(package + 4);
				WORD crc = *(package + 6);
				break;

			case 15:
			case 16:
				WORD start_adr = *(package + 2);
				WORD quantity = *(package + 4);
				BYTE byte_count = *(package + 6);
				WORD crc = *(package + 6 + byte_count + 1);
		}
	}

	void parseRTURespPackage(char* package) {
		BYTE slave = *package;
		BYTE func = *(package + 1);
		switch (func) {
			case 1:
			case 2:
			case 3:
			case 4:
				WORD start_adr = *(package + 2);
				WORD quantity = *(package + 4);
				WORD crc = *(package + 6); 
				break;
			
			case 5:
			case 6:
				WORD adr = *(package + 2);
				WORD val = *(package + 4);
				WORD crc = *(package + 6);
				break;

			case 15:
			case 16:
				WORD start_adr = *(package + 2);
				WORD quantity = *(package + 4);
				BYTE byte_count = *(package + 6);
				WORD crc = *(package + 6 + byte_count + 1);
				break;
			
			case 129:
			case 130:
			case 131:
			case 132:
			case 133:
			case 134:
			case 143:
			case 144:
				BYTE exception_code = *(package + 2);
				WORD crc = *(package + 3);
		}
	}



private:
	WORD m_start_adr; 	// Стартовый адрес
	WORD m_end_adr;		// Последний доступный адрес карты
	WORD m_quantity;		// Количество регистров

	MapType m_map_type;	// Тип карты адресов, WORD или BIT
	MemMode m_mem_mode;  // Тип хранения в памяти, big endian(byte swap|on|off)|little endian(byte swap|on|off)

	BIT* m_mem_8_ptr;   	// Указатель BIT на начало области карты памяти 
	WORD* m_mem_16_ptr; 	// Указатель WORD на начало области карты памяти

	std::string m_name; 	// Наименование карты памяти

	bool m_bind; 			// Привязана ли карта памяти к внешнему указателю на память или нет (например на структуру пользователя)
								// В этом случае чтение и запись в пользовательскую память необходимо будет производить только через картку памяти Map, т.к. здесь есть mutex
								// только через картку памяти Map, если хотим безопасную работу с данными за счет mutex

	std::mutex m_mtx;		// Мьютекс для разделения доступа при запросах разными потоками
};

} // data
} // mb

#endif // MB_LINGUIST_H