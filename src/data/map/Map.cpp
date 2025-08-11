#include "Map.h"

#include <new> // для std::bad_alloc

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
		m_mem_8_ptr = new (std::nothrow) BIT[m_quantity];
		if (m_mem_8_ptr == nullptr) {
			result = false;
		}
	}
	// Если карта слов WORD_MAP
	else {
		m_mem_16_ptr = new (std::nothrow) WORD[m_quantity];
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
	if (adr < m_start_adr || adr < m_end_adr || val == nullptr) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	WORD offset = adr - m_start_adr;
	*val = *(m_mem_16_ptr + offset);
	return true;
}

bool Map::readDWord(WORD adr, DWORD *const val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (adr < m_start_adr || val == nullptr || m_end_adr < adr + 1) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	WORD offset = adr - m_start_adr;
	*val = *(reinterpret_cast<DWORD*>(m_mem_16_ptr + offset));
	return true;
}

bool Map::readWords(const WORD adr, const WORD quantity, WORD *const val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (adr < m_start_adr || val == nullptr || quantity == 0 || m_end_adr < adr + quantity - 1) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	WORD offset = adr - m_start_adr;
	for (WORD i = 0; i < quantity; i++) {
		*(val + i) = *(m_mem_16_ptr + offset + i);
	}
	return true;
}

bool Map::writeWord(const WORD adr, const WORD val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (adr < m_start_adr || adr > m_end_adr) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	WORD offset = adr - m_start_adr;
	*(m_mem_16_ptr + offset) = val;
	return true;
}

bool Map::writeDWord(const WORD adr, const DWORD val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (adr < m_start_adr || m_end_adr < adr + 1) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	WORD offset = adr - m_start_adr;
	*(reinterpret_cast<DWORD*>(m_mem_16_ptr + offset)) = val;
	return true;
}

bool Map::writeWords(const WORD adr, const WORD quantity, WORD *const val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (adr < m_start_adr || val == nullptr || quantity == 0 || m_end_adr < adr + quantity - 1) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	WORD offset = adr - m_start_adr;
	for (WORD i = 0; i < quantity; i++) {
		*(m_mem_16_ptr + offset + i) = *(val + i);
	}
	return true;
}

bool Map::readBit(const WORD adr, BIT *const val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (val == nullptr || (adr < m_start_adr || adr > m_end_adr) && m_map_type == MapType::BIT_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	// Если битовая карта BIT
	if (m_map_type == MapType::BIT_MAP) {
		WORD offset = adr - m_start_adr;
		*val = *(m_mem_8_ptr + offset);
	}
	// Если карта слов WORD, рассчитываем какой бы это был WORD относительно адреса бита,
	// ищем адрес WORD и номер бита, вычитываем данный бит
	else {
		WORD word_adr = adr / WORD_BIT_SIZE;
		if (word_adr < m_start_adr || word_adr > m_end_adr) return false;
		WORD bit_number = adr % WORD_BIT_SIZE;

		WORD offset = word_adr - m_start_adr;
		WORD word_val = *(m_mem_16_ptr + offset);
		*val = (word_val >> bit_number) & 1;
	}
	return true;
}

bool Map::readBits(const WORD adr, const WORD quantity, BIT *const val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (val == nullptr || quantity == 0 || (adr < m_start_adr || m_end_adr < adr + quantity - 1) && m_map_type == MapType::BIT_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	// Если битовая карта BIT
	if (m_map_type == MapType::BIT_MAP) {
		WORD offset = adr - m_start_adr;
		for (WORD i = 0; i < quantity; i++) {
			*(val + i) = *(m_mem_8_ptr + offset + i);
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
				word_val = *(m_mem_16_ptr + offset); 	// Получаем значение слова из памяти
			}
			*val = (word_val >> bit_number) & 1; 		// Получаем установленный бит по номеру
			++bit_number;
		}
	}
	
	return true;
}

bool Map::writeBit(const WORD adr, BIT val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if ((adr < m_start_adr || adr > m_end_adr) && m_map_type == MapType::BIT_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);

	if (val > 0) val = 1;
	else val = 0;

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
			*(m_mem_8_ptr + offset + i) = *(val + i);
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
	if (val == nullptr || (adr < m_start_adr || adr > m_end_adr) && m_map_type == MapType::BIT_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	// Если битовая карта BIT
	if (m_map_type == MapType::BIT_MAP) {
		WORD offset = adr - m_start_adr;
		*val = *(m_mem_8_ptr + offset);
	}
	// Если карта слов WORD, uint8 по указанному адресу
	else {
		if (adr < m_start_adr || adr < m_end_adr || val == nullptr) return false;
		WORD offset = adr - m_start_adr;
		*val = *(m_mem_16_ptr + offset);
		return true;
	}
	return true;
}

bool Map::readUInt16(const WORD adr, uint16_t * const val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (adr < m_start_adr || adr < m_end_adr || val == nullptr) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	WORD offset = adr - m_start_adr;
	*val = *(m_mem_16_ptr + offset);
	return true;
}

bool Map::readUInt32(const WORD adr, uint32_t *const val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (adr < m_start_adr || val == nullptr || m_end_adr < adr + 1) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	WORD offset = adr - m_start_adr;
	*val = *(reinterpret_cast<DWORD*>(m_mem_16_ptr + offset));
	return true;
}

bool Map::readInt8(const WORD adr, int8_t *const val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (val == nullptr || (adr < m_start_adr || adr > m_end_adr) && m_map_type == MapType::BIT_MAP) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	// Если битовая карта BIT
	if (m_map_type == MapType::BIT_MAP) {
		WORD offset = adr - m_start_adr;
		*val = *(m_mem_8_ptr + offset);
	}
	// Если карта слов WORD, uint8 по указанному адресу
	else {
		if (adr < m_start_adr || adr < m_end_adr || val == nullptr) return false;
		WORD offset = adr - m_start_adr;
		*val = *(m_mem_16_ptr + offset);
		return true;
	}
	return true;
}

bool Map::readInt16(const WORD adr, int16_t * const val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (adr < m_start_adr || adr < m_end_adr || val == nullptr) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	WORD offset = adr - m_start_adr;
	*val = *(m_mem_16_ptr + offset);
	return true;
}

bool Map::readInt32(const WORD adr, int32_t *const val, MemMode mode) {
	std::lock_guard<std::mutex> lock(m_mtx);
	if (adr < m_start_adr || val == nullptr || m_end_adr < adr + 1) return false;
	if (mode == MemMode::LITTLE_ENDIAN_BYTE_SWAP_MODE);
	WORD offset = adr - m_start_adr;
	*val = *(reinterpret_cast<DWORD*>(m_mem_16_ptr + offset));
	return true;
}





} // data
} // mb