#ifndef MB_MAP_H
#define MB_MAP_H

#include <vector>
#include <cstdint> 
#include <string>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <mutex>

#define DWORD  uint32_t
#define WORD   uint16_t
#define BIT    uint8_t

namespace mb {
namespace data {

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

static constexpr MemMode default_mem_mode = MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE;

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
	Map() {}
	Map(WORD start_adr, WORD quantity) : m_start_adr(start_adr), m_quantity(quantity) {
		m_end_adr = m_start_adr + quantity - 1;
	}
	~Map() {}

	bool bindMap(MapType map_type, WORD start_adr, WORD quantity, void *data_ptr); // Привязка карты памяти к внешнему указателю на память (например на структуру пользователя)
																										  // В этом случае чтение и запись в пользовательскую память необходимо будет производить
																										  // только через картку памяти Map, если хотим безопасную работу с данными за счет mutex

	bool initNewMemory(); 											// Инициализация новой памяти
	void clearMemory();												// Очистка выделенной памяти

	// void setStartAdr(WORD start_adr);   						// Установка стартового адреса
	// void setQuantity(WORD quantity);	 							// Установка количества регистров

	/* Чтение|Запись битов и слов */
	bool readWord(WORD adr, WORD *val, MemMode mode = default_mem_mode);
	bool readDWord(WORD adr, DWORD *val, MemMode mode = default_mem_mode);
	bool readWords(WORD adr, WORD quantity, WORD *val, MemMode mode = default_mem_mode);

	bool writeWord(WORD adr, WORD *val, MemMode mode = default_mem_mode);
	bool writeDWord(WORD adr, DWORD *val, MemMode mode = default_mem_mode);
	bool writeWords(WORD adr, WORD quantity, WORD *val, MemMode mode = default_mem_mode);

	bool readBit(WORD adr, BIT *val, MemMode mode = default_mem_mode);
	bool readBits(WORD adr, WORD quantity, BIT *val, MemMode mode = default_mem_mode);

	bool writeBit(WORD adr, BIT *val, MemMode mode = default_mem_mode);
	bool writeBits(WORD adr, WORD quantity, BIT *val, MemMode mode = default_mem_mode);

	/* Чтение|Запись и интерпретирование в нужное представление int signed|int unsigned|float */
	bool readUInt8(WORD adr, uint8_t* val, MemMode mode = default_mem_mode);
	bool readUInt16(WORD adr, uint16_t* val, MemMode mode = default_mem_mode);
	bool readUInt32(WORD adr, uint32_t* val, MemMode mode = default_mem_mode);
	bool readInt8(WORD adr, int8_t* val, MemMode mode = default_mem_mode);
	bool readInt16(WORD adr, int16_t* val, MemMode mode = default_mem_mode);
	bool readInt32(WORD adr, int32_t* val, MemMode mode = default_mem_mode);
	bool readFloat16(WORD adr, float *val, uint8_t precision, MemMode mode = default_mem_mode);
	bool readFloat32(WORD adr, float* val, MemMode mode = default_mem_mode);
	
	bool writeFloat32(WORD adr, float* val, MemMode mode = default_mem_mode);

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

#endif // MB_MAP_H