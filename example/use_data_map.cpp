#include <iostream>

#include "Map.h"

int main(void) {

	mb::data::Map m(1,100); // Создаем карту памяти начальный адрес 1 в количестве 100 шт.
	m.initNewMemory();

	// Записываем адрес 0 бит 1, получаем ошибку, т.к. не получается
	if (!m.writeWord(0, 1)) {
		std::cout << "Error write 0 adr" << std::endl;
	}


	if (!m.writeWord(101, 1)) {
		std::cout << "Error write 101 adr" << std::endl;
	}

	uint8_t v1,v2; 
	m.readBit(22, &v1);
	m.readBit(23, &v2);

	std::cout << static_cast<int>(v1) << std::endl;
	std::cout << static_cast<int>(v2) << std::endl;

	return 0;
}