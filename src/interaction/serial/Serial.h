#ifndef MB_SERIAL_H
#define MB_SERIAL_H

#include <stdint.h>
#include <sys/time.h>
#include <termios.h>
#include <string>

namespace mb {
namespace interaction {

/* Timeouts in microsecond (0.5 s) */
#define _RESPONSE_TIMEOUT 500000
#define _BYTE_TIMEOUT     500000

class Serial {
public:
	Serial();
	Serial(std::string device, int baud, char parity, int data_bit, int stop_bit);
	~Serial();

	void init(std::string device, int baud, char parity, int data_bit, int stop_bit);
	int connect();
	bool isConnected();
	long send(const uint8_t *req, int req_length);
	int waitReceive(uint8_t *msg);
	long doReceive();
	void setRts(bool flag);
	int flush();
	// _modbus_rtu_select(modbus_t *ctx, fd_set *rset, struct timeval *tv, int length_to_read)
	int doSelect(fd_set *rset, struct timeval *tv, int length_to_read);
	int closeSocket();

	static speed_t getTermiosSpeed(int baud);

private:
	int m_fd;

	std::string m_device;
	int m_baudrate;
	char m_parity;
	uint8_t m_data_bit;
	uint8_t m_stop_bit;

	timeval m_response_timeout;
	timeval m_byte_timeout;
	timeval m_indication_timeout;

	termios m_old_tios;

	bool m_debug;
};

}
}

#endif // MB_SERIAL_H