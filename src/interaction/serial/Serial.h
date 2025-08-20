#ifndef MB_SERIAL_H
#define MB_SERIAL_H

#include <stdint.h>
#include <sys/time.h>
#include <termios.h>
#include <string>

namespace mb {
namespace interaction {

enum class RtsMode {
	NONE = 0,
	UP   = 1,
	DOWN = 2
};

enum class SerialMode {
	NONE  = 0,
	RS232 = 1,
	RS485 = 2
};

/* Timeouts in microsecond (0.5 s) */
#define _RESPONSE_TIMEOUT 500000
#define _BYTE_TIMEOUT     500000

class Serial {
public:
	Serial();
	Serial(std::string device, int baud, char parity, int data_bit, int stop_bit);

	// Инициализация
	void init(std::string device, int baud, char parity, int data_bit, int stop_bit);
	// Подключение
	bool connect();
	// Статус подключения
	bool isConnected();
	// Отправка сообщения
	long send(const uint8_t *req, int req_length);
	// Отправка сообщения с помощью rts
	long sendRts(const uint8_t *req, int req_length);
	// Получение сообщения
	long receive(uint8_t *rsp, int rsp_length);
	// Установка аппаратного управления потока данных
	bool setRts(RtsMode mode);
	// Переключить аппаратное управление потока данных
	bool invertRts(bool flag);
	// Установка режима для аппаратной части RS232/RS485 (если поддерживается)
	bool setSerialMode(SerialMode mode);
	// Очистка буфера ввода (принимаемые данные) вывода (не отправленные данные)
	bool flush();
	// Ожидание файлового дескриптора на ввод данных
	bool doSelect(fd_set *rset, int milliseconds);
	// Закрыть файловый дескриптор
	void doClose();
	// Получить дескриптор/сокет устройства
	int getSocket();

	// Проверка поддержки аппаратного управления
	static bool isSerialSupported(int socket);
	// Проверка поддержки аппаратного управления RTS
	static bool isRtsSupported(int socket);
	// Получение скорости
	static speed_t getTermiosSpeed(int baud, bool debug);

private:
	// Дескриптор устройства
	int m_fd;

	// Путь к устройству
	std::string m_device;
	// Скорость в бодах
	int m_baudrate;
	// Четность
	char m_parity;
	// Биты данных
	uint8_t m_data_bit;
	// Стоп бит
	uint8_t m_stop_bit;

	// Для сохранения старых настроек последовательного порта
	termios m_old_tios;

	// Режим rts (если поддераживается)
	RtsMode m_rts;
	// Задержка rts
	int m_rts_delay;
	// Интервал передачи одного байта в rts
	int m_onebyte_time;

	// Режим последовательного порта (если поддерижвается)
	SerialMode m_serial_mode;

	// Отладочные сообщения 
	bool m_debug;
};

}
}

#endif // MB_SERIAL_H