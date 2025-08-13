#include <iostream>

#include "Map.h"

int main(void) {

	/****** Тест BIT_MAP ******/

	mb::data::Map m(1,100); // Создаем карту памяти начальный адрес 1 в количестве 100 шт.
	m.initNewMemory(mb::data::MapType::BIT_MAP);

	std::cout << "                  Init bit map start adr 1, quantity 100" << std::endl;
	m.printBitMap(10);

	/** readBit | writeBit **/
	// Записываем адрес 0 бит 1, получаем ошибку, т.к. не находится не в диапазоне
	if (!m.writeBit(0, 1)) {
		std::cout << "Map: Error writeBit 0 adr out of range" << std::endl;
	}
	else {
		std::cout << "Map: writeBit 0 adr bit 1" << std::endl;
	}

	// Записываем адрес 0 бит 1, получаем ошибку, т.к. не находится не в диапазоне
	if (!m.writeBit(101, 1)) {
		std::cout << "Map: Error writeBit 101 adr out of range" << std::endl;
	}
	else {
		std::cout << "Map: writeBit 101 adr bit 1" << std::endl;
	}

	uint8_t bit_val;

	// Запись в 22 адрес бита, значения 11
	if (!m.writeBit(22, 11)) {
		std::cout << "Map: Error writeBit 22 adr" << std::endl;
	}
	else {
		std::cout << "Map: writeBit 22 adr val 11 > 0 (means bit 1)" << std::endl;
	}

	// Чтение адреса 22
	if (!m.readBit(22, &bit_val)) {
		std::cout << "Map: Error readBit 22 adr" << std::endl;
	}
	else {
		std::cout << "Map: readBit 22 adr, value " << static_cast<int>(bit_val) << std::endl;
	}


	/** readBits | writeBits **/

	const int BITS_VALS_SIZE = 3;
	uint8_t write_bit_vals[] = { 1, 0, 1 };
	uint8_t read_bit_vals[BITS_VALS_SIZE];

	// Запись в 54 адрес массива битов
	if (!m.writeBits(54, BITS_VALS_SIZE, write_bit_vals)) {
		std::cout << "Map: Error writeBits 54 adr" << std::endl;
	}
	else {
		std::cout << "Map: writeBits 54 adr, quantity 3, array values" << std::endl;
	}

	// Чтение адреса 54, 3 бита подряд
	if (!m.readBits(54, BITS_VALS_SIZE, read_bit_vals)) {
		std::cout << "Map: Error readBits 54 adr" << std::endl;
	}
	else {
		std::cout << "Map: readBits 54, quantity 3, array values" << std::endl;
	}

	for (int i = 0; i < BITS_VALS_SIZE; i++) {
		std::cout << "Map: Bit adr " << static_cast<int>(i + 54) << " value " << static_cast<int>(read_bit_vals[i]) << std::endl;
	}

	std::cout << "                  Writed bit map (adr 22, 54-57)" << std::endl;
	m.printBitMap(10);


	/****** Тест WORD_MAP ******/

	/** readWord | readDWord | readWords | writeWord | writeDWord | writeWords **/

	// Переназначаем память под word
	m.initNewMemory(0, 100, mb::data::MapType::WORD_MAP);

	std::cout << "                  Init word map start adr 0, quantity 100" << std::endl;
	m.printWordMap(10);

	// Записываем адрес 0 слово 1, получаем ошибку, т.к. не находится не в диапазоне
	if (!m.writeWord(0, 33)) {
		std::cout << "Map: Error writeWord 0 adr out of range" << std::endl;
	}
	else {
		std::cout << "Map: writeWord 0 adr, value 33" << std::endl;
	}

	// Записываем адрес 102 слово 1, получаем ошибку, т.к. не находится не в диапазоне
	if (!m.writeWord(102, 15)) {
		std::cout << "Map: Error writeWord 102 adr out of range" << std::endl;
	}
	else {
		std::cout << "Map: writeWord 102 adr value 15" << std::endl;
	}

	// Записываем адрес 76 слово 27
	if (!m.writeWord(76, 27)) {
		std::cout << "Map: Error writeWord 76 adr" << std::endl;
	}
	else {
		std::cout << "Map: writeWord 76 adr value 27" << std::endl;
	}

	uint16_t word_read_val;
	
	// Читаем адрес 76
	if (!m.readWord(76, &word_read_val)) {
		std::cout << "Map: Error readWord 76 adr" << std::endl;
	}
	else {
		std::cout << "Map: readWord 76 adr" << std::endl;
	}

	std::cout << "Map: Word adr 76 value " << static_cast<int>(word_read_val) << std::endl;

	// Записываем адрес 61 двойное слово 999999
	if (!m.writeDWord(61, 999999)) {
		std::cout << "Map: Error writeDWord 61 adr" << std::endl;
	}
	else {
		std::cout << "Map: writeDWord 61 adr value 999999" << std::endl;
	}

	uint32_t dword_read_val;
	
	// Читаем адрес 61
	if (!m.readDWord(61, &dword_read_val)) {
		std::cout << "Map: Error readDWord 61 adr" << std::endl;
	}
	else {
		std::cout << "Map: readDWord 61 adr" << std::endl;
	}

	std::cout << "Map: Word adr 61 value " << static_cast<uint32_t>(dword_read_val) << std::endl;

	const int WORD_VALS_SIZE = 5;
	uint16_t write_word_vals[] = {99, 111, 25, 7, 3};
	// Записываем адрес 20 последовательно 5 слов 
	if (!m.writeWords(20, WORD_VALS_SIZE, write_word_vals)) {
		std::cout << "Map: Error writeWords 20 adr" << std::endl;
	}
	else {
		std::cout << "Map: writeWords 20 adr quantity 5 values {99, 111, 25, 7, 3}" << std::endl;
	}

	uint16_t read_word_vals[WORD_VALS_SIZE];
	// Читаем адрес 20
	if (!m.readWords(20, WORD_VALS_SIZE, read_word_vals)) {
		std::cout << "Map: Error readWords 20 adr" << std::endl;
	}
	else {
		std::cout << "Map: readWords adr 20 quantity 5" << std::endl;
	}

	for (int i = 0; i < WORD_VALS_SIZE; i++) {
		std::cout << "Map: Word adr " << static_cast<uint16_t>(i + 20) << " value " << static_cast<int>(read_word_vals[i]) << std::endl;
	}

	std::cout << "                  Writed word map (76, 61, 62, 20, 21, 22, 23, 24)" << std::endl;
	m.printWordMap(10);

	/** Тестирование битов в контексте WORD_MAP (читаются и записываются отдельные биты слова) */
	/** readWordBit | readWordBits | writeWordBit | writeWordBits **/

	// Запишем по адресу 8 число 7335 в битовом представлении 0001 1100 1010 0111
	if (!m.writeWord(8, 7335)) {
		std::cout << "Map: Error writeWord 8 adr" << std::endl;
	}
	else {
		std::cout << "Map: writeWord 8 adr value 7335" << std::endl;
	}
	
	// // Запишем в адрес 9 значение 7334
	if (!m.writeWord(9,7334)) {
		std::cout << "Map: Error writeWord word 9, val 7334" << std::endl;
	}
	else {
		std::cout << "Map: writeWord word 9, val 7334" << std::endl;
	}

	// 1 слово = 16 бит, 16 * 8 = 128 адрес бита, + начальный адрес бита (1), 
	//  итого наше битовое слово начинается с адреса 128
	// Карта памяти (стартовый адрес = 1)
	//  bit adr  0-15   16-31   32-47  48-63    64-79  80-95   96-111 112-127 128-143 144-159
	// word adr   0       1       2       3       4       5       6      7       8       9
	// 		  [     0][     0][     0][     0][     0][     0][     0][     0][     0][     0]

	// Запишем по адресу 8 число 7335 в битовом представлении 0001 1100 1010 0111
	std::cout << "Map: word 7335 = 0001 1100 1010 0111, read this word with readWordBit and readWordBits" << std::endl;

	// Здесь при чтении мы увидим что считанные биты совпадают с битовым представлением 0001 1100 1010 0111 числа 7335
	uint8_t wbit_val;
	WORD wbit_adr = 128;
	for (int i = 0; i < 16; i++) {
		if (!m.readWordBit(wbit_adr + i,&wbit_val)) {
			std::cout << "Map: Error readWordBit " << static_cast<uint16_t>(wbit_adr+i) << " adr" << std::endl;
		}
		std::cout << "Map: readWordBit " << static_cast<uint16_t>(wbit_adr+i) << " adr, value " << static_cast<uint16_t>(wbit_val) << std::endl;
	}

	uint8_t wbit_vals[32];
	if (!m.readWordBits(wbit_adr,32,wbit_vals)) {
		std::cout << "Map: Error readWordBits " << static_cast<uint16_t>(wbit_adr) << " adr" << std::endl;
	}
	else {
		std::cout << "Map: readWordBits " << static_cast<uint16_t>(wbit_adr) << " adr, quantity 32" << std::endl;
	}

	std::cout << "Map: readWordBits adr bit 128 (adr word 8), quantity 32 value 7335: ";
	for (int i = 0; i < 32; i++) {
		if (i % 8 == 0) printf(" ");
		printf("%d",static_cast<int>(wbit_vals[i]));
	}
	std::cout << std::endl;

	m.printWordMapBits(5);

	uint8_t write_wbits_values[] = {1,1,0,0,1,1};

	// 53 word adr 15 bit position, 863 bit adr
	if (!m.writeWordBits(863,6,write_wbits_values)) {
		std::cout << "Map: Error writeWordBits 863 adr (53 word adr 15 bit pos)" << std::endl;
	}
	else {
		std::cout << "Map: writeWordBits 863 adr (53 word adr 15 bit pos), quantity 6, vals 1,1,0,0,1,1" << std::endl;
	}

	if (!m.writeWordBit(860,1)) {
		std::cout << "Map: Error writeWordBit 860 adr (53 word adr 12 bit pos)" << std::endl;
	}
	else {
		std::cout << "Map: writeWordBit 860 adr (53 word adr 12 bit pos), val 1" << std::endl;
	}

	m.printWordMapBits(5);

	/** Тестирование битов в контексте WORD_MAP (читаются и записываются отдельные биты слова) */
	/** readWordNBit | readWordNBits | writeWordNBit | writeWordNBits **/

	for (int i = 0; i < 16; i++) {
		if (!m.readWordNBit(8,i,&wbit_val)) {
			std::cout << "Map: Error readWordNBit 8, number bit " << i << std::endl;
		}
		std::cout << "Map: readWordNBit adr 8, number bit " << i << ": " << static_cast<uint16_t>(wbit_val) << std::endl;
	}
	
	uint8_t wbits_vals[32];
	if (!m.readWordNBits(8,15,32,wbits_vals)) {
		std::cout << "Map: Error readWordNBits 8, number bit 15, quantity 32 " << std::endl;
	}
	else {
		std::cout << "Map: readWordNBits 8, number bit 15, quantity 32 " << std::endl;
	}

	std::cout << "Map: readWordNBits Word 7335 bits: ";
	for (int i = 0; i < 32; i++) {
		if (i % 8 == 0) printf(" ");
		printf("%d",static_cast<int>(wbits_vals[i]));
	}
	std::cout << std::endl;

	if (!m.writeWordNBit(8,0,0)) {
		std::cout << "Map: Error writeWordNBit 8, number bit 0, val 0" << std::endl;
	}
	else {
		std::cout << "Map: writeWordNBit 8, number bit 0, val 0" << std::endl;
	}

	if (!m.writeWordNBit(10,0,1)) {
		std::cout << "Map: Error writeWordNBit word 10, number bit 0, val 1" << std::endl;
	}
	else {
		std::cout << "Map: writeWordNBit word 10, number bit 0, val 1" << std::endl;
	}

	if (!m.writeWordNBit(10,2,1)) {
		std::cout << "Map: Error writeWordNBit 10, number bit 2, val 1" << std::endl;
	}
	else {
		std::cout << "Map: writeWordNBit 10, number bit 2, val 1" << std::endl;
	}

	uint8_t write_wbits_vals[32] = {1};
	for (int i = 0; i < 32; i++) {
		write_wbits_vals[i] = 1;
	}
	write_wbits_vals[1] = 0;
	write_wbits_vals[2] = 0;
	write_wbits_vals[3] = 0;

	if (!m.writeWordNBits(11,6,32,write_wbits_vals)) {
		std::cout << "Map: Error writeWordNBits 11, number bit 6, quantity 32" << std::endl;
	}
	else {
		std::cout << "Map: writeWordNBits 11, number bit 6, quantity 32" << std::endl;
	}

	m.printWordMapBits(5);

	return 0;
}