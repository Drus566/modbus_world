#include "Map.h"

#include <new> // для std::bad_alloc
#include <math.h>

namespace mb {
namespace data {

bool Map::bindMap(MapType map_type, WORD start_adr, WORD quantity, void* data_ptr) {
	if (data_ptr == nullptr) {
		return false;
	}

	m_map_type = map_type; // Устанавливаем тип карты
	m_start_adr = start_adr; // Устанавливаем стартовый адрес
	m_quantity = quantity; // Устанавливаем количество

	// Если карта битов BIT_MAP
	if (m_map_type == MapType::BIT_MAP) m_mem_8_ptr = static_cast<BIT*>(data_ptr);
	// Если карта слов WORD_MAP
	else m_mem_16_ptr = static_cast<WORD*>(data_ptr);

	m_bind = true; // Переключаем привязку

	return true;
}

bool Map::initNewMemory(WORD start_adr, WORD quantity, MapType map_type, MemMode mem_mode) {
	m_start_adr = start_adr;
	m_quantity = quantity;
	return initNewMemory(map_type, mem_mode);
}

bool Map::initNewMemory(MapType map_type, MemMode mem_mode) {
	bool result = true;

	m_map_type = map_type;
	m_mem_mode = mem_mode;
	
	clearMemory();

	if (m_quantity == 0) {
		return false;
	}

	// Если карта битов BIT_MAP
	if (m_map_type == MapType::BIT_MAP) {
		// Выделяем память и заполняем нулями с помощью () на конце
		m_mem_8_ptr = new (std::nothrow) BIT[m_quantity];
		if (m_mem_8_ptr == nullptr) {
			result = false;
		}
	}
	// Если карта слов WORD_MAP
	else {
		// Выделяем память и заполняем нулями с помощью () на конце
		m_mem_16_ptr = new (std::nothrow) WORD[m_quantity]();
		if (m_mem_16_ptr == nullptr) {
			result = false;
		}
	}
	return result;
}

bool Map::initNewMemory() {
	return initNewMemory(MapType::WORD_MAP, MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
}

void Map::clearMemory() {
	if (m_mem_16_ptr != nullptr) {
		delete[] m_mem_16_ptr;
		m_mem_16_ptr = nullptr;
	}
	if (m_mem_8_ptr != nullptr) {
		delete[] m_mem_8_ptr;
		m_mem_8_ptr = nullptr;
	}
}

void Map::setMapType(MapType map_type) {
	m_map_type = map_type;
}

bool Map::printBitMap(WORD width) {
	if (m_quantity <= 0 || m_map_type == MapType::WORD_MAP) return false;
	printf("    Adr");
	for (int i = 0; i < m_quantity; i++) {
		if (i % width == 0) printf("\n%6d ", m_start_adr + i);
		printf("[%2d]", *(m_mem_8_ptr + i));
	}
	printf("\n\n");
	return true;
}

bool Map::printWordMap(WORD width) {
	if (m_quantity <= 0 || m_map_type == MapType::BIT_MAP) return false;
	printf("    Adr");
	for (int i = 0; i < m_quantity; i++) {
		if (i % width == 0)  printf("\n%6d ", m_start_adr + i);
		printf("[%6d]", *(m_mem_16_ptr + i));
	}
	printf("\n\n");
	return true;
}

bool Map::printWordMapBits(WORD width) {
	if (m_quantity <= 0 || m_map_type == MapType::BIT_MAP) return false;
	uint16_t val;
	printf("    Adr");
	for (int i = 0; i < m_quantity; i++) {
		if (i % width == 0)  printf("\n%6d ", m_start_adr + i);
		val = *(m_mem_16_ptr + i);
		printf("[");
		for (int i = 15; i >= 0; i--) { // с самого старшего бита к младшему
			if (i == 7) printf(" ");
        	printf("%c", (val & (1 << i)) ? '1' : '0');
    	}
		printf("]");
	}
	printf("\n\n");
	return true;
}


// void Map::setStartAdr(WORD start_adr) {
// 	std::lock_guard<std::mutex> lock(m_mtx);
// 	m_start_adr = start_adr; 
// 	if (m_quantity > 0) m_end_adr = m_start_adr + m_quantity - 1;
// }

// void Map::setQuantity(WORD quantity) {
// 	std::lock_guard<std::mutex> lock(m_mtx);
// 	m_quantity = quantity; 
// 	m_end_adr = m_start_adr + m_quantity - 1;
// }

bool Map::readWord(const WORD adr, WORD * const val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (adr < m_start_adr || m_end_adr < adr || val == nullptr || m_map_type == MapType::BIT_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	WORD offset = adr - m_start_adr;
	*val = *(m_mem_16_ptr + offset);
	return true;
}

bool Map::readDWord(WORD adr, DWORD *const val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (adr < m_start_adr || val == nullptr || m_end_adr < adr + 1 || m_map_type == MapType::BIT_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	WORD offset = adr - m_start_adr;
	*val = *(reinterpret_cast<DWORD*>(m_mem_16_ptr + offset));
	return true;
}

bool Map::readWords(const WORD adr, const WORD quantity, WORD *const val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (adr < m_start_adr || val == nullptr || quantity == 0 || m_end_adr < adr + quantity - 1 || m_map_type == MapType::BIT_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	WORD offset = adr - m_start_adr;
	for (WORD i = 0; i < quantity; i++) {
		*(val + i) = *(m_mem_16_ptr + offset + i);
	}
	return true;
}

bool Map::writeWord(const WORD adr, const WORD val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (adr < m_start_adr || adr > m_end_adr || m_map_type == MapType::BIT_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	WORD offset = adr - m_start_adr;
	*(m_mem_16_ptr + offset) = val;
	return true;
}

bool Map::writeDWord(const WORD adr, const DWORD val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (adr < m_start_adr || m_end_adr < adr + 1 || m_map_type == MapType::BIT_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	WORD offset = adr - m_start_adr;
	*(reinterpret_cast<DWORD*>(m_mem_16_ptr + offset)) = val;
	return true;
}

bool Map::writeWords(const WORD adr, const WORD quantity, WORD *const val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (adr < m_start_adr || val == nullptr || quantity == 0 || m_end_adr < adr + quantity - 1 || m_map_type == MapType::BIT_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	WORD offset = adr - m_start_adr;
	for (WORD i = 0; i < quantity; i++) {
		*(m_mem_16_ptr + offset + i) = *(val + i);
	}
	return true;
}

bool Map::readWordNBit(const WORD word_adr, const WORD bit_number, BIT *const val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	
	if (val == nullptr || bit_number >= WORD_BIT_SIZE || m_map_type == MapType::BIT_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);

	if (word_adr < m_start_adr || word_adr > m_end_adr) return false;

	WORD offset = word_adr - m_start_adr;
	WORD word_val = *(m_mem_16_ptr + offset);
	*val = (word_val >> bit_number) & 1;
	return true;
}

bool Map::readWordNBits(const WORD word_adr, const WORD bit_number, const WORD quantity, BIT *const val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (val == nullptr || bit_number >= WORD_BIT_SIZE || m_map_type == MapType::BIT_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);

	WORD offset = word_adr - m_start_adr;
	WORD word_val = *(m_mem_16_ptr + offset);
	WORD t_bit_number = bit_number;
	WORD t_word_counter = 0;

	WORD end_word_adr = word_adr + ((bit_number + quantity - 1) / WORD_BIT_SIZE);
	bool new_word = false;

	if (word_adr < m_start_adr || end_word_adr > m_end_adr) return false;

	for (WORD i = 0; i < quantity; i++) {
		new_word = t_bit_number == WORD_BIT_SIZE;
		if (new_word && i != 0) {
			++t_word_counter;
			offset = word_adr - m_start_adr + t_word_counter;
			word_val = *(m_mem_16_ptr + offset);
			t_bit_number = 0;
		}
		*(val + i) = helperReadWordBit(word_val, t_bit_number); 		// Получаем установленный бит по номеру
		++t_bit_number;
	}
	return true;
}

bool Map::writeWordNBit(const WORD word_adr, const WORD bit_number, const BIT val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	
	if (bit_number >= WORD_BIT_SIZE || m_map_type == MapType::BIT_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);

	if (word_adr < m_start_adr || word_adr > m_end_adr) return false;

	WORD offset = word_adr - m_start_adr;
	WORD word_val = *(m_mem_16_ptr + offset);
	*(m_mem_16_ptr + offset) = helperWriteWordBit(word_val,bit_number,val);
	return true;
}

bool Map::writeWordNBits(const WORD word_adr, const WORD bit_number, const WORD quantity, BIT *const val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (val == nullptr || bit_number >= WORD_BIT_SIZE || m_map_type == MapType::BIT_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);

	WORD offset = word_adr - m_start_adr;
	WORD word_val = *(m_mem_16_ptr + offset);
	WORD t_bit_number = bit_number;
	WORD t_word_counter = 0;

	WORD end_word_adr = word_adr + ((bit_number + quantity - 1) / WORD_BIT_SIZE);
	bool new_word = false;

	if (word_adr < m_start_adr || end_word_adr > m_end_adr) return false;

	for (WORD i = 0; i < quantity; i++) {
		new_word = t_bit_number % WORD_BIT_SIZE == 0;
		if (new_word && i != 0) {
			*(m_mem_16_ptr + offset) = word_val;
			++t_word_counter;
			offset = word_adr - m_start_adr + t_word_counter; // Получаем сдвиг в памяти
			word_val = *(m_mem_16_ptr + offset); 	// Получаем значение слова из памяти
			t_bit_number = 0;
		}
		word_val = helperWriteWordBit(word_val,t_bit_number,*(val + i));
		++t_bit_number;
	}
	*(m_mem_16_ptr + offset) = word_val;
	return true;
}

bool Map::readWordBit(const WORD bit_adr, BIT* val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	
	if (val == nullptr || m_map_type == MapType::BIT_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);

	// Рассчитываем какой бы это был WORD относительно адреса бита,
	// ищем адрес WORD и номер бита, вычитываем данный бит
	WORD word_adr = bit_adr / WORD_BIT_SIZE;
	if (word_adr < m_start_adr || word_adr > m_end_adr) return false;
	WORD bit_number = bit_adr % WORD_BIT_SIZE;

	WORD offset = word_adr - m_start_adr;
	WORD word_val = *(m_mem_16_ptr + offset);
	*val = (word_val >> bit_number) & 1;
	return true;
}

bool Map::readWordBits(const WORD bit_adr, const WORD quantity, BIT *const val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (val == nullptr || m_map_type == MapType::BIT_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);

	WORD word_adr = bit_adr / WORD_BIT_SIZE;		// Адрес слова соответствующий адресу бита
	WORD bit_number = bit_adr % WORD_BIT_SIZE;	 	// Номер бита в слове
	WORD offset = word_adr - m_start_adr;
	WORD word_val = *(m_mem_16_ptr + offset);

	// Проверяем входит ли в диапазоны
	WORD end_word_adr = (bit_adr + quantity - 1) / WORD_BIT_SIZE;
	bool new_word = false;

	if (word_adr < m_start_adr || end_word_adr > m_end_adr) return false;

	for (WORD i = 0; i < quantity; i++) {
		new_word = bit_number == WORD_BIT_SIZE;
		if (new_word && i != 0) {
			++word_adr;
			offset = word_adr - m_start_adr;		 	// Получаем сдвиг в памяти
			word_val = *(m_mem_16_ptr + offset); 		// Получаем значение слова из памяти
			bit_number = 0;
		}
		*(val + i) = helperReadWordBit(word_val, bit_number); 		// Получаем установленный бит по номеру
		++bit_number;
	}
	return true;
}

bool Map::writeWordBit(const WORD bit_adr, const BIT val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	
	if (m_map_type == MapType::BIT_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);

	// Рассчитываем какой бы это был WORD относительно адреса бита,
	// ищем адрес WORD и номер бита, вычитываем данный бит
	WORD word_adr = bit_adr / WORD_BIT_SIZE;
	if (word_adr < m_start_adr || word_adr > m_end_adr) return false;
	WORD bit_number = bit_adr % WORD_BIT_SIZE;

	WORD offset = word_adr - m_start_adr;
	WORD word_val = *(m_mem_16_ptr + offset);
	*(m_mem_16_ptr + offset) = helperWriteWordBit(word_val,bit_number,val);
	return true;
}

bool Map::writeWordBits(const WORD bit_adr, const WORD quantity, BIT *const val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (val == nullptr || m_map_type == MapType::BIT_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);

	WORD word_adr = bit_adr / WORD_BIT_SIZE;		// Адрес слова соответствующий адресу бита
	WORD bit_number = bit_adr % WORD_BIT_SIZE;	 	// Номер бита в слове
	WORD offset = word_adr - m_start_adr;
	WORD word_val = *(m_mem_16_ptr + offset);

	// Проверяем входит ли в диапазоны
	WORD end_word_adr = (bit_adr + quantity - 1) / WORD_BIT_SIZE;
	bool new_word = false;

	if (word_adr < m_start_adr || end_word_adr > m_end_adr) return false;

	for (WORD i = 0; i < quantity; i++) {
		new_word = bit_number == WORD_BIT_SIZE;
		if (new_word && i != 0) {
			*(m_mem_16_ptr + offset) = word_val; 	// Запись в память нового регистра
			++word_adr; 							// Инкременитруем слово
			offset = word_adr - m_start_adr;		// Получаем сдвиг в памяти
			word_val = *(m_mem_16_ptr + offset); 	// Получаем значение слова из памяти
			bit_number = 0;							// Меняем позицию бита на ноль
		}
		word_val = helperWriteWordBit(word_val,bit_number,*(val + i));
		++bit_number;
	}
	*(m_mem_16_ptr + offset) = word_val;
	return true;
}

bool Map::readBit(const WORD adr, BIT *const val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (val == nullptr || (adr < m_start_adr || adr > m_end_adr) || m_map_type == MapType::WORD_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	
	WORD offset = adr - m_start_adr;
	*val = *(m_mem_8_ptr + offset);
	
	return true;
}

bool Map::readBits(const WORD adr, const WORD quantity, BIT *const val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (val == nullptr || quantity == 0 || (adr < m_start_adr || m_end_adr < adr + quantity - 1) || m_map_type == MapType::WORD_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	WORD offset = adr - m_start_adr;
	for (WORD i = 0; i < quantity; i++) {
		*(val + i) = *(m_mem_8_ptr + offset + i);
	}
		
	return true;
}

bool Map::writeBit(const WORD adr, BIT val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if ((adr < m_start_adr || adr > m_end_adr) && m_map_type == MapType::BIT_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);

	val > 0 ? val = 1 : val = 0;

	// Если битовая карта BIT
	if (m_map_type == MapType::BIT_MAP) {
		WORD offset = adr - m_start_adr;
		*(m_mem_8_ptr + offset) = val;
	}
	// Если карта слов WORD, рассчитываем какой бы это был WORD относительно адреса бита,
	// ищем адрес WORD и номер бита, вычитываем данный бит
	else {
		WORD word_adr = adr / WORD_BIT_SIZE;
		if (word_adr < m_start_adr || word_adr > m_end_adr) return false;
		WORD bit_number = adr % WORD_BIT_SIZE;

		WORD offset = word_adr - m_start_adr;
		WORD word_val = *(m_mem_16_ptr + offset);
		if (val) {
			*(m_mem_16_ptr + offset) |= (1 << bit_number);
		}
		else {
			*(m_mem_16_ptr + offset) &= ~(1 << bit_number);
		}
	}
	return true;
}


bool Map::writeBits(const WORD adr, const WORD quantity, BIT *const val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (val == nullptr || quantity == 0 || (adr < m_start_adr || m_end_adr < adr + quantity - 1) && m_map_type == MapType::BIT_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	// Если битовая карта BIT
	if (m_map_type == MapType::BIT_MAP) {
		WORD offset = adr - m_start_adr;
		for (WORD i = 0; i < quantity; i++) {
			BIT cur_val = *(val + i);
			cur_val > 0 ? cur_val = 1 : cur_val = 0;
			*(m_mem_8_ptr + offset + i) = cur_val;
		}
	}
	// Если битовая карта WORD
	else {
		WORD word_adr;
		WORD offset;
		WORD word_val;
		WORD bit_number = 0;

		// Проверяем входит ли в диапазоны
		word_adr = adr / WORD_BIT_SIZE; // Получаем адрес слова
		WORD end_word_adr = (adr + quantity - 1) / WORD_BIT_SIZE;

		if (word_adr < m_start_adr || end_word_adr > m_end_adr) return false;

		for (WORD i = 0; i < quantity; i++) {
			if (bit_number > WORD_BIT_SIZE - 1 || i == 0) {
				word_adr = adr / WORD_BIT_SIZE; 			// Получаем адрес слова
				bit_number = adr % WORD_BIT_SIZE;	 	// Получаем номер бита
				offset = word_adr - m_start_adr;		 	// Получаем сдвиг в памяти
				*(m_mem_16_ptr + offset) = word_val; // Получаем значение слова из памяти
			}
			*val = (word_val >> bit_number) & 1; 		// Получаем установленный бит по номеру
			++bit_number;
		}
	}
	
	return true;
}

bool Map::readUInt8(const WORD adr, uint8_t *const val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (adr < m_start_adr || m_end_adr < adr || val == nullptr || m_map_type == MapType::BIT_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	WORD offset = adr - m_start_adr;
	*val = *(m_mem_16_ptr + offset);
	return true;
}

bool Map::readUInt16(const WORD adr, uint16_t * const val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (adr < m_start_adr || m_end_adr < adr || val == nullptr || m_map_type == MapType::BIT_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	WORD offset = adr - m_start_adr;
	*val = *(m_mem_16_ptr + offset);
	return true;
}

bool Map::readUInt32(const WORD adr, uint32_t *const val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (adr < m_start_adr || val == nullptr || m_end_adr < adr + 1 || m_map_type == MapType::BIT_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	WORD offset = adr - m_start_adr;
	*val = *(reinterpret_cast<DWORD*>(m_mem_16_ptr + offset));
	return true;
}

bool Map::readInt8(const WORD adr, int8_t *const val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (adr < m_start_adr || m_end_adr < adr || val == nullptr || m_map_type == MapType::BIT_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	WORD offset = adr - m_start_adr;
	*val = *(m_mem_16_ptr + offset);
	return true;
}

bool Map::readInt16(const WORD adr, int16_t * const val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (adr < m_start_adr || m_end_adr < adr || val == nullptr || m_map_type == MapType::BIT_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	WORD offset = adr - m_start_adr;
	*val = *(m_mem_16_ptr + offset);
	return true;
}

bool Map::readInt32(const WORD adr, int32_t *const val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (adr < m_start_adr || val == nullptr || m_end_adr < adr + 1 || m_map_type == MapType::BIT_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	WORD offset = adr - m_start_adr;
	*val = *(reinterpret_cast<DWORD*>(m_mem_16_ptr + offset));
	return true;
}

bool Map::readFloat16(const WORD adr, float *const val, uint8_t precision, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (adr < m_start_adr || m_end_adr < adr || val == nullptr || m_map_type == MapType::BIT_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	WORD offset = adr - m_start_adr;
	*val = (((int16_t)*(m_mem_16_ptr + offset)) / pow(10, precision));
	return true;
}

bool Map::readFloat32(const WORD adr, float *const val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (adr < m_start_adr || val == nullptr || m_end_adr < adr + 1 || m_map_type == MapType::BIT_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	WORD offset = adr - m_start_adr;
	*val = *(reinterpret_cast<float*>(m_mem_16_ptr + offset));
	return true;
}

bool Map::writeUInt8(const WORD adr, const uint8_t val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (adr < m_start_adr || m_end_adr < adr || m_map_type == MapType::BIT_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	WORD offset = adr - m_start_adr;
	*(m_mem_16_ptr + offset) = val;
	return true;
}

bool Map::writeUInt16(const WORD adr, const uint16_t val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (adr < m_start_adr || m_end_adr < adr || m_map_type == MapType::BIT_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	WORD offset = adr - m_start_adr;
	*(m_mem_16_ptr + offset) = val;
	return true;
}

bool Map::writeUInt32(const WORD adr, const uint32_t val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (adr < m_start_adr  || m_end_adr < adr + 1 || m_map_type == MapType::BIT_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	WORD offset = adr - m_start_adr;
	*(reinterpret_cast<DWORD*>(m_mem_16_ptr + offset)) = val; 
	return true;
}

bool Map::writeInt8(const WORD adr, const int8_t val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (adr < m_start_adr || m_end_adr < adr || m_map_type == MapType::BIT_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	WORD offset = adr - m_start_adr;
	 *(m_mem_16_ptr + offset) = val;
	return true;
}

bool Map::writeInt16(const WORD adr, const int16_t val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (adr < m_start_adr || m_end_adr < adr || m_map_type == MapType::BIT_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	WORD offset = adr - m_start_adr;
	*(m_mem_16_ptr + offset) = val; 
	return true;
}

bool Map::writeInt32(const WORD adr, const int32_t val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (adr < m_start_adr || m_end_adr < adr + 1 || m_map_type == MapType::BIT_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	WORD offset = adr - m_start_adr;
	*(reinterpret_cast<DWORD*>(m_mem_16_ptr + offset)) = val; 
	return true;
}

bool Map::writeFloat16(const WORD adr, const float val, uint8_t precision, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (adr < m_start_adr || m_end_adr < adr || m_map_type == MapType::BIT_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	WORD offset = adr - m_start_adr;
	*(m_mem_16_ptr + offset) = (int16_t)(val * powf(10, precision)); 
	return true;
}

bool Map::writeFloat32(const WORD adr, const float val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (adr < m_start_adr || m_end_adr < adr + 1 || m_map_type == MapType::BIT_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	WORD offset = adr - m_start_adr;
	*(reinterpret_cast<float*>(m_mem_16_ptr + offset)) = val; 
	return true;
}


} // data
} // mb