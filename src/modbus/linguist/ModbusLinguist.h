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

class Map {
public:
	Map() : m_mem_16_ptr(nullptr), m_mem_8_ptr(nullptr) {}
	Map(WORD start_adr, WORD quantity) : m_start_adr(start_adr), 
													 m_quantity(quantity),
													 m_mem_16_ptr(nullptr),
													 m_mem_8_ptr(nullptr) {
		m_end_adr = m_start_adr + quantity - 1;
	}
	~Map() {}

	bool bindMap(MapType map_type, WORD start_adr, WORD quantity, void *data_ptr); // Привязка карты памяти к внешнему указателю на память (например на структуру пользователя)
																										  // В этом случае чтение и запись в пользовательскую память необходимо будет производить
																										  // только через картку памяти Map, если хотим безопасную работу с данными за счет mutex
	
	bool initNewMemory(WORD start_adr, WORD quantity, MapType map_type, MemMode mode = default_mem_mode); 		  // Инициализация новой памяти
	bool initNewMemory(MapType map_type, MemMode mode = default_mem_mode); 		  
	bool initNewMemory(); 														  
	
	void clearMemory();												// Очистка выделенной памяти

	void setMapType(MapType map_type);							// Установка типа карты WORD или BIT

	// void setStartAdr(WORD start_adr);   						// Установка стартового адреса
	// void setQuantity(WORD quantity);	 							// Установка количества регистров

	/* Чтение|Запись битов и слов */
	// Чтение слова
	bool readWord(const WORD adr, WORD *const val, MemMode mode = default_mem_mode);
	// Чтение двойного слова
	bool readDWord(const WORD adr, DWORD *const val, MemMode mode = default_mem_mode);
	// Чтение массива слов
	bool readWords(const WORD adr, const WORD quantity, WORD *const val, MemMode mode = default_mem_mode);

	// Запись слова
	bool writeWord(const WORD adr, const WORD val, MemMode mode = default_mem_mode);
	// Запись двойного слова
	bool writeDWord(const WORD adr, const DWORD val, MemMode mode = default_mem_mode);
	// Запись массива слов
	bool writeWords(const WORD adr, const WORD quantity, WORD *const val, MemMode mode = default_mem_mode);

	// Чтение указанного номера бита в слове, отсчет битов начинается с 0
	bool readWordNBit(const WORD word_adr, const WORD bit_number, BIT *const val, MemMode mode = default_mem_mode);
	// Чтение массива битов начиная с указанного номера бита в слове, отсчет битов начинается с 0
	bool readWordNBits(const WORD word_adr, const WORD bit_number, const WORD quantity, BIT *const val, MemMode mode = default_mem_mode);
	// Запись указанного номера бита в слове, отсчет битов начинается с 0
	bool writeWordNBit(const WORD word_adr, const WORD bit_number, const BIT val, MemMode mode = default_mem_mode);
	// Запись массива битов начиная с указанного номера бита в слове, отсчет битов начинается с 0
	bool writeWordNBits(const WORD word_adr, const WORD bit_number, const WORD quantity, BIT *const val, MemMode mode = default_mem_mode);

	// Чтение бита по адресу в карте слов, первый адрес бита равен 0, отсчет идет от 0 адреса в глобальном понимании
	// Например есть WORD_MAP стартовый адрес = 7, количество = 10, тогда адрес первого бита слова будет считаться как 7 * 16 = 112 (слово по адресу 7)
	bool readWordBit(const WORD adr, BIT* val, MemMode mode = default_mem_mode);
	// Чтение массива битов по адресу в карте слов, первый адрес бита равен 0, отсчет идет от 0 адреса в глобальном понимании, и далее по порядку
	bool readWordBits(const WORD adr, const WORD quantity, BIT *const val, MemMode mode = default_mem_mode);
	// Запись бита по адресу в карте слов
	bool writeWordBit(const WORD adr, const BIT val, MemMode mode = default_mem_mode);
	// Запись битов по адресу в карте слов
	bool writeWordBits(const WORD adr, const WORD quantity, BIT *const val, MemMode mode = default_mem_mode);

	// Чтение бита
	bool readBit(const WORD adr, BIT* val, MemMode mode = default_mem_mode);
	// Чтение массива битов
	bool readBits(const WORD adr, const WORD quantity, BIT *const val, MemMode mode = default_mem_mode);

	// Запись бита
	bool writeBit(const WORD adr, const BIT val, MemMode mode = default_mem_mode);
	// Запись массива битов
	bool writeBits(const WORD adr, const WORD quantity, BIT *const val, MemMode mode = default_mem_mode);

	/* Чтение|Запись и интерпретирование в нужное представление int signed|int unsigned|float */
	bool readUInt8(const WORD adr, uint8_t *const val, MemMode mode = default_mem_mode);
	bool readUInt16(const WORD adr, uint16_t *const val, MemMode mode = default_mem_mode);
	bool readUInt32(const WORD adr, uint32_t *const val, MemMode mode = default_mem_mode);
	bool readInt8(const WORD adr, int8_t *const val, MemMode mode = default_mem_mode);
	bool readInt16(const WORD adr, int16_t *const val, MemMode mode = default_mem_mode);
	bool readInt32(const WORD adr, int32_t *const val, MemMode mode = default_mem_mode);
	bool readFloat16(const WORD adr, float *const val, uint8_t precision = 1, MemMode mode = default_mem_mode);
	bool readFloat32(const WORD adr, float *const val, MemMode mode = default_mem_mode);

	bool writeUInt8(const WORD adr, const uint8_t val, MemMode mode = default_mem_mode);
	bool writeUInt16(const WORD adr, const uint16_t val, MemMode mode = default_mem_mode);
	bool writeUInt32(const WORD adr, const uint32_t val, MemMode mode = default_mem_mode);
	bool writeInt8(const WORD adr, const int8_t val, MemMode mode = default_mem_mode);
	bool writeInt16(const WORD adr, const int16_t val, MemMode mode = default_mem_mode);
	bool writeInt32(const WORD adr, const int32_t val, MemMode mode = default_mem_mode);
	bool writeFloat16(const WORD adr, const float val, uint8_t precision = 1, MemMode mode = default_mem_mode);
	bool writeFloat32(const WORD adr, const float val, MemMode mode = default_mem_mode);

	bool printBitMap(WORD width); // Вывод карты битов в консоль
	bool printWordMap(WORD width); // Вывод карты слов в консоль
	bool printWordMapBits(WORD width); // Вывод карты слов в консоль в битовом представлении

	inline WORD helperWriteWordBit(WORD word, BIT bit_number, BIT bit_val) {
		WORD result;
		if (bit_val) result = word | (1 << bit_number); // Установка 1
		else result = word & ~(1 << bit_number);		// Установка 0
		return result;
	}

	inline BIT helperReadWordBit(WORD word, BIT bit_number) {
		return (word >> bit_number) & 1;	
	}

	inline WORD helperInvertWordBit(WORD word, BIT bit_number) {
		return word = word ^ (1 << bit_number);
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