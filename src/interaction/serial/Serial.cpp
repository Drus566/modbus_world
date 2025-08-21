#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <sys/ioctl.h>
#include <linux/serial.h>

#include "Serial.h"
#include "Time.h"

namespace mb {
namespace interaction {

#define DEFAULT_PORT "/dev/ttyS0"
#define DEFAULT_BAUD 9600
#define DEFAULT_PARITY 'N'

Serial::Serial() {}

Serial::Serial(std::string device, int baud, char parity, int data_bit, int stop_bit) {
	init(device, baud, parity, data_bit, stop_bit);
}

int Serial::getSocket() { return m_fd; }

void Serial::init(std::string device, int baud, char parity, int data_bit, int stop_bit) {
	if (device.empty()) {
		std::cout << "Device string is empty, then set to " << DEFAULT_PORT << std::endl;
		m_device = DEFAULT_PORT;
	}
	else m_device = device;

	if (baud <= 0) {
		std::cout << "Baudrate value is <= 0, then set to " << DEFAULT_BAUD << std::endl;
		m_baudrate = DEFAULT_BAUD;
	}
	else m_baudrate = baud;

	if (parity == 'N' || parity == 'E' || parity == 'O') m_parity = parity;
	else {
		m_parity = 'N';
		std::cout << "Parity is incorrect, then set to " << DEFAULT_PARITY << std::endl;
	}

	m_data_bit = data_bit;
	m_stop_bit = stop_bit;

	m_serial_mode = SerialMode::NONE;

	m_rts = RtsMode::NONE;
	m_onebyte_time = 1000000 * (1 + m_data_bit + (m_parity == 'N' ? 0 : 1) + m_stop_bit) / m_baudrate;
	m_rts_delay = m_onebyte_time;
}

bool Serial::connect() {
	struct termios tios;
	int flags;
	speed_t speed;

	if (m_debug) {
		std::cout << "Serial: Opening "<< m_device << " at " 
		<< m_baudrate << " bauds (" << m_parity << ", " 
		<< (int)m_data_bit << ", " << (int)m_stop_bit << ")" << std::endl;
	}

	flags = O_RDWR | O_NOCTTY | O_NONBLOCK | O_EXCL;
	m_fd = open(m_device.c_str(), flags);

	if (m_fd < 0) {
		if (m_debug) {
			std::cout << "Serial: Error open device " << m_device << " (" << strerror(errno) << ")" << std::endl;;
		}
		return false;
	}

	tcgetattr(m_fd, &m_old_tios);
	memset(&tios, 0, sizeof(termios));

	if (9600 == B9600) speed = m_baudrate;
	else speed = Serial::getTermiosSpeed(m_baudrate, m_debug);

	if ((cfsetispeed(&tios,speed) < 0) || (cfsetospeed(&tios,speed) < 0)) {
		close(m_fd);
		m_fd = 1;
		return false;
	}

	tios.c_cflag |= (CREAD | CLOCAL);

	tios.c_cflag &= ~CSIZE;
	switch (m_data_bit) {
	case 5:
		tios.c_cflag |= CS5;
		break;
	case 6:
		tios.c_cflag |= CS6;
		break;
	case 7:
		tios.c_cflag |= CS7;
		break;
	case 8:
	default:
		tios.c_cflag |= CS8;
		break;
	}

	if (m_stop_bit == 1) tios.c_cflag &= ~CSTOPB;
	else tios.c_cflag |= CSTOPB;

	if (m_parity == 'N') tios.c_cflag &= ~PARENB;
	else if (m_parity == 'E') {
		tios.c_cflag |= PARENB;
		tios.c_cflag &= ~PARODD;
	}
	else {
		tios.c_cflag |= PARENB;
		tios.c_cflag |= PARODD;
	}

	tios.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

	if (m_parity == 'N') tios.c_iflag &= ~INPCK;
	else tios.c_iflag |= INPCK;

	tios.c_iflag &= ~(IXON | IXOFF | IXANY);
	tios.c_oflag &= ~OPOST;

	tios.c_cc[VMIN] = 0;
	tios.c_cc[VTIME] = 0;

	if (tcsetattr(m_fd, TCSANOW, &tios) < 0) {
		close(m_fd);
		m_fd = -1;
		return false;
	}

	return true;
}

bool Serial::isConnected() { return (m_fd >= 0); }

long Serial::send(const uint8_t* req, int req_length) {
	return write(m_fd, req, req_length);
}

long Serial::sendRts(const uint8_t* req, int req_length) {
	ssize_t size;

	if (m_debug) {
		std::cout << "Sending request using RTS signal\n" << std::endl;
	}

	invertRts(m_rts == RtsMode::UP);
	usleep(m_rts_delay);

	size = write(m_fd, req, req_length);

	usleep(m_onebyte_time * req_length + m_rts_delay);
	invertRts(m_rts != RtsMode::UP);

	return size;
}

long Serial::receive(uint8_t* rsp, int rsp_length) {
	return read(m_fd, rsp, rsp_length);
}

bool Serial::setSerialMode(SerialMode mode) {
	if (m_fd < 0) {
		errno = EINVAL;
		std::cout << "Serial: Error setSerialMode, Device socket " << m_fd << " not opened" << std::endl;
		return false;
	}

	if (!Serial::isSerialSupported(m_fd)) {
		if (errno == ENOTSUP) {
			std::cout << "Serial: Serial mode not supported" << std::endl;
		}
		else {
			std::cout << "Serial: ioctl error call" << std::endl;
		}
		return false;
	}

	serial_rs485 rs485_conf;
	if (mode == SerialMode::RS485) {
		// Get
		if (ioctl(m_fd, TIOCGRS485, &rs485_conf) < 0) return false;
		
		// Set
		rs485_conf.flags |= SER_RS485_ENABLED;
		if (ioctl(m_fd, TIOCSRS485, &rs485_conf) < 0) return false;

		m_serial_mode = SerialMode::RS485;
	}
	else if (mode == SerialMode::RS232) {
		/* Turn off RS485 mode only if required */
		if (m_serial_mode == SerialMode::RS485) {
			/* The ioctl call is avoided because it can fail on some RS232 ports */
			if (ioctl(m_fd, TIOCGRS485, &rs485_conf) < 0) return false;
			
			rs485_conf.flags &= ~SER_RS485_ENABLED;
			if (ioctl(m_fd, TIOCSRS485, &rs485_conf) < 0) return false;
		}
		m_serial_mode = SerialMode::RS232;
	}
	return true;
}

bool Serial::setRts(RtsMode mode) {
	if (m_fd <= 0) {
		errno = EINVAL;
		std::cout << "Serial: Error setRts, Device socket " << m_fd << " not opened" << std::endl;
		return false;
	}

	if (!Serial::isRtsSupported(m_fd)) {
		std::cout << "Serial: Rts mode not supported" << std::endl;
		return false;
	}

	if (mode == RtsMode::NONE || mode == RtsMode::UP || mode == RtsMode::DOWN) {
		m_rts = mode;
		invertRts(m_rts != RtsMode::UP);
	}
}

bool Serial::invertRts(bool flag) {
	int fd = m_fd;
	int flags;

	ioctl(fd, TIOCMGET, &flags);
	if (flag) flags |= TIOCM_RTS;
	else flags &= ~TIOCM_RTS;
	ioctl(fd, TIOCMSET, &flags);
}

bool Serial::flush() { return tcflush(m_fd, TCIOFLUSH) == 0; }

bool Serial::doSelect(fd_set* rset, int milliseconds) {
	timeval tv;
	mb::helpers::millisecondsToTimeval(milliseconds, tv);

	int s_rc;
	while ((s_rc = select(m_fd + 1, rset, NULL, NULL, &tv)) == -1) {
		if (errno == EINTR) {
			if (m_debug) std::cout << "Serial: A non blocked signal was caught" << std::endl;
			/* Necessary after an error */
			FD_ZERO(rset);
			FD_SET(m_fd, rset);
		}
		else return false;
	}

	if (s_rc == 0) {
		/* Timeout */
		errno = ETIMEDOUT;
		return false;
	}
	return true;
}

void Serial::doClose() {
	if (m_fd >= 0) {
		tcsetattr(m_fd, TCSANOW, &m_old_tios);
		close(m_fd);
		m_fd = -1;
	}
}

void Serial::setDebug(bool flag) {
	m_debug = flag;
}

bool Serial::isRtsSupported(int socket) {
	bool result = true;
	int status;
	if (ioctl(socket, TIOCMGET, &status) == -1) {
		result = false;
		errno = ENOTSUP;
	}
	return result;
}

bool Serial::isSerialSupported(int socket) {
	bool result = true;
	struct serial_rs485 rs485_conf;
	memset(&rs485_conf, 0, sizeof(rs485_conf));
	int ret = ioctl(socket, TIOCGRS485, &rs485_conf);
	if (ret < 0) {
		if (errno == ENOTTY || errno == EINVAL) {
			errno = ENOTSUP;
		}
		result = false;
	}
	return result;
}

speed_t Serial::getTermiosSpeed(int baud, bool debug) {
	speed_t speed;

	switch (baud) {
	case 110:
		speed = B110;
		break;
	case 300:
		speed = B300;
		break;
	case 600:
		speed = B600;
		break;
	case 1200:
		speed = B1200;
		break;
	case 2400:
		speed = B2400;
		break;
	case 4800:
		speed = B4800;
		break;
	case 9600:
		speed = B9600;
		break;
	case 19200:
		speed = B19200;
		break;
	case 38400:
		speed = B38400;
		break;
#ifdef B57600
	case 57600:
		speed = B57600;
		break;
#endif
#ifdef B115200
	case 115200:
		speed = B115200;
		break;
#endif
#ifdef B230400
	case 230400:
		speed = B230400;
		break;
#endif
#ifdef B460800
	case 460800:
		speed = B460800;
		break;
#endif
#ifdef B500000
	case 500000:
		speed = B500000;
		break;
#endif
#ifdef B576000
	case 576000:
		speed = B576000;
		break;
#endif
#ifdef B921600
	case 921600:
		speed = B921600;
		break;
#endif
#ifdef B1000000
	case 1000000:
		speed = B1000000;
		break;
#endif
#ifdef B1152000
	case 1152000:
		speed = B1152000;
		break;
#endif
#ifdef B1500000
	case 1500000:
		speed = B1500000;
		break;
#endif
#ifdef B2500000
	case 2500000:
		speed = B2500000;
		break;
#endif
#ifdef B3000000
	case 3000000:
		speed = B3000000;
		break;
#endif
#ifdef B3500000
	case 3500000:
		speed = B3500000;
		break;
#endif
#ifdef B4000000
	case 4000000:
		speed = B4000000;
		break;
#endif
	default:
		speed = B9600;
		if (debug) std::cout << "WARNING Unknown baud rate " << baud << " (B9600 used)" << std::endl;
	}

	return speed;
}


// Установка аппаратного управления потока данных
// bool Serial::setRts(Serial &serial, RtsMode mode) {
// 	if (serial.m_fd <= 0) {
// 		errno = EINVAL;
// 		std::cout << "Serial: Error setRts, Device socket " << serial.m_fd << " not opened" << std::endl;
// 		return false;
// 	}

// 	if (!Serial::isRtsSupported(serial.m_fd)) {
// 		std::cout << "Serial: Rts mode not supported" << std::endl;
// 		return false;
// 	}

// 	if (mode == RtsMode::NONE || mode == RtsMode::UP || mode == RtsMode::DOWN) {
// 		serial.m_rts = mode;
// 		Serial::invertRts(serial, serial.m_rts != RtsMode::UP);
// 	}
// }

// // Переключить аппаратное управление потока данных
// bool Serial::invertRts(Serial &serial, bool flag) {
// 	int fd = serial.m_fd;
// 	int flags;

// 	ioctl(fd, TIOCMGET, &flags);
// 	if (flag) flags |= TIOCM_RTS;
// 	else flags &= ~TIOCM_RTS;
// 	ioctl(fd, TIOCMSET, &flags);
// }

// // Установка режима для аппаратной части RS232/RS485 (если поддерживается)
// bool Serial::setSerialMode(Serial& serial, SerialMode mode) {
// 	if (serial.m_fd < 0) {
// 		errno = EINVAL;
// 		std::cout << "Serial: Error setSerialMode, Device socket " << serial.m_fd << " not opened" << std::endl;
// 		return false;
// 	}

// 	if (!Serial::isSerialSupported(serial.m_fd)) {
// 		if (errno == ENOTSUP) {
// 			std::cout << "Serial: Serial mode not supported" << std::endl;
// 		}
// 		else {
// 			std::cout << "Serial: ioctl error call" << std::endl;
// 		}
// 		return false;
// 	}

// 	serial_rs485 rs485_conf;
// 	if (mode == SerialMode::RS485) {
// 		// Get
// 		if (ioctl(serial.m_fd, TIOCGRS485, &rs485_conf) < 0) return false;
		
// 		// Set
// 		rs485_conf.flags |= SER_RS485_ENABLED;
// 		if (ioctl(serial.m_fd, TIOCSRS485, &rs485_conf) < 0) return false;

// 		serial.m_serial_mode = SerialMode::RS485;
// 	}
// 	else if (mode == SerialMode::RS232) {
// 		/* Turn off RS485 mode only if required */
// 		if (serial.m_serial_mode == SerialMode::RS485) {
// 			/* The ioctl call is avoided because it can fail on some RS232 ports */
// 			if (ioctl(serial.m_fd, TIOCGRS485, &rs485_conf) < 0) return false;

// 			rs485_conf.flags &= ~SER_RS485_ENABLED;
// 			if (ioctl(serial.m_fd, TIOCSRS485, &rs485_conf) < 0) return false;
// 		}
// 		serial.m_serial_mode = SerialMode::RS232;
// 	}
// 	return true;
// }

// long Serial::sendRts(Serial &serial, const uint8_t *req, int req_length) {
// 	ssize_t size;

// 	if (serial.m_debug) {
// 		std::cout << "Sending request using RTS signal\n" << std::endl;
// 	}

// 	Serial::invertRts(serial, serial.m_rts == RtsMode::UP);
// 	usleep(serial.m_rts_delay);

// 	size = write(serial.m_fd, req, req_length);

// 	usleep(serial.m_onebyte_time * req_length + serial.m_rts_delay);
// 	Serial::invertRts(serial, serial.m_rts != RtsMode::UP);

// 	return size;
// }

/* Waits a response from a modbus server or a request from a modbus client.
	This function blocks if there is no replies (3 timeouts).

	The function shall return the number of received characters and the received
	message in an array of uint8_t if successful. Otherwise it shall return -1
	and errno is set to one of the values defined below:
	- ECONNRESET
	- EMBBADDATA
	- ETIMEDOUT
	- read() or recv() error codes
*/

// int Serial::waitReceive(uint8_t *msg, int length_to_read) {
// 	int rc;						// результат выполнения
// 	fd_set rset;				// дескрипторы для select
// 	struct timeval tv;		// время для таймаутов
// 	struct timeval *p_tv;	// указатель на время для таймаутов
// 	int msg_length = 0;		// считано байтов

// 	// Проверка соединения
// 	if (!isConnected()) {
// 		if (m_debug) {
// 			std::cout << "ERROR The connection is not established" << std::endl;
// 		}
// 		return -1;
// 	}

// 	/* Add a file descriptor to the set */
// 	FD_ZERO(&rset);
// 	FD_SET(m_fd, &rset);

// 	int ret = 0;
// 	while (length_to_read != 0) {
// 		rc = doSelect(&rset, p_tv);
// 		if (rc == -1) {
// 			std::cout << "doSelect error" << std::endl;
// 			return -1;
// 		}

// 		rc = doReceive(msg + msg_length, length_to_read);
// 		if (rc == 0) {
// 			errno = ECONNRESET;
// 			rc = -1;
// 		}

// 		if (rc == -1) {
// 			// _error_print(ctx, "read");
// 			if ((ctx->error_recovery & MODBUS_ERROR_RECOVERY_LINK) &&
// 				 (errno == ECONNRESET || errno == ECONNREFUSED || errno == EBADF))
// 			{
// 				closeSocket();
// 				// int saved_errno = errno;
// 				// modbus_close(ctx);
// 				// modbus_connect(ctx);
// 				/* Could be removed by previous calls */
// 				// errno = saved_errno;
// 			}
// 		}

// 		/* Display the hex code of each character received */
// 		if (m_debug) {
// 			int i;
// 			for (i = 0; i < rc; i++)
// 				printf("<%.2X>", msg[msg_length + i]);
// 		}

// 		/* Sums bytes received */
// 		msg_length += rc;
// 		/* Computes remaining bytes */
// 		length_to_read -= rc;

// 		if (length_to_read == 0) {
// 			// switch (step)
// 			// {
// 			// case _STEP_FUNCTION:
// 			// 	/* Function code position */
// 			// 	length_to_read = compute_meta_length_after_function(
// 			// 		 msg[ctx->backend->header_length], msg_type);
// 			// 	if (length_to_read != 0)
// 			// 	{
// 			// 		step = _STEP_META;
// 			// 		break;
// 			// 	} /* else switches straight to the next step */
// 			// case _STEP_META:
// 			// 	length_to_read = compute_data_length_after_meta(ctx, msg, msg_type);
// 			// 	if ((msg_length + length_to_read) > ctx->backend->max_adu_length)
// 			// 	{
// 			// 		errno = EMBBADDATA;
// 			// 		_error_print(ctx, "too many data");
// 			// 		return -1;
// 			// 	}
// 			// 	step = _STEP_DATA;
// 			// 	break;
// 			// default:
// 			// 	break;
// 			// }
// 		}

// 		if (length_to_read > 0 && (m_byte_timeout.tv_sec > 0 || m_byte_timeout.tv_usec > 0)) {
// 			/* If there is no character in the buffer, the allowed timeout
// 				interval between two consecutive bytes is defined by
// 				byte_timeout */
// 			tv.tv_sec = m_byte_timeout.tv_sec;
// 			tv.tv_usec = m_byte_timeout.tv_usec;
// 			p_tv = &tv;
// 		}
// 		/* else timeout isn't set again, the full response must be read before
// 			expiration of response timeout (for CONFIRMATION only) */
// 	}
// }


// 	// long send()
// 	// {
// 		return write(ctx->s, req, req_length);
// 		// return 0;
// 	// }
// //

/* Waits a response from a modbus server or a request from a modbus client.
	This function blocks if there is no replies (3 timeouts).

	The function shall return the number of received characters and the received
	message in an array of uint8_t if successful. Otherwise it shall return -1
	and errno is set to one of the values defined below:
	- ECONNRESET
	- EMBBADDATA
	- ETIMEDOUT
	- read() or recv() error codes
*/

// 	// int _modbus_receive_msg(modbus_t *ctx, uint8_t *msg, msg_type_t msg_type)
// 	// {
// 		// int rc;
// 		// fd_set rset;
// 		// struct timeval tv;
// 		// struct timeval *p_tv;
// 		// unsigned int length_to_read;
// 		// int msg_length = 0;
// 		// _step_t step;
// // #ifdef _WIN32
// 		// int wsa_err;
// // #endif
// // 
// 		// if (ctx->debug)
// 		// {
// 			// if (msg_type == MSG_INDICATION)
// 			// {
// 				// printf("Waiting for an indication...\n");
// 			// }
// 			// else
// 			// {
// 				// printf("Waiting for a confirmation...\n");
// 			// }
// 		// }
// // 
// 		// if (!ctx->backend->is_connected(ctx))
// 		// {
// 			// if (ctx->debug)
// 			// {
// 				// fprintf(stderr, "ERROR The connection is not established.\n");
// 			// }
// 			// if (ctx->log)
// 			// {
// 				// sprintf(ctx->error_log_buffer, "The connection is not established");
// 			// }
// 			// return -1;
// 		// }
// // 
// 		// /* Add a file descriptor to the set */
// 		// FD_ZERO(&rset);
// 		// FD_SET(ctx->s, &rset);
// // 
// 		// /* We need to analyse the message step by step.  At the first step, we want
// 		//  * to reach the function code because all packets contain this
// 		//  * information. */
// 		// step = _STEP_FUNCTION;
// 		// length_to_read = ctx->backend->header_length + 1;
// // 
// 		// if (msg_type == MSG_INDICATION)
// 		// {
// 			// /* Wait for a message, we don't know when the message will be received */
// 			// if (ctx->indication_timeout.tv_sec == 0 && ctx->indication_timeout.tv_usec == 0)
// 			// {
// 				// /* By default, the indication timeout isn't set */
// 				// p_tv = NULL;
// 			// }
// 			// else
// 			// {
// 				// /* Wait for an indication (name of a received request by a server, see schema)
// 				//  */
// 				// tv.tv_sec = ctx->indication_timeout.tv_sec;
// 				// tv.tv_usec = ctx->indication_timeout.tv_usec;
// 				// p_tv = &tv;
// 			// }
// 		// }
// 		// else
// 		// {
// 			// tv.tv_sec = ctx->response_timeout.tv_sec;
// 			// tv.tv_usec = ctx->response_timeout.tv_usec;
// 			// p_tv = &tv;
// 		// }
// // 
// 		// int ret = 0;
// 		// while (length_to_read != 0)
// 		// {
// 			// rc = ctx->backend->select(ctx, &rset, p_tv, length_to_read);
// 			// if (rc == -1)
// 			// {
// 				// _error_print(ctx, "select");
// 				// if (ctx->error_recovery & MODBUS_ERROR_RECOVERY_LINK)
// 				// {
// // #ifdef _WIN32
// 					// wsa_err = WSAGetLastError();
// // 
// 					no equivalent to ETIMEDOUT when select fails on Windows
// 					// if (wsa_err == WSAENETDOWN || wsa_err == WSAENOTSOCK)
// 					// {
// 						// modbus_close(ctx);
// 						// modbus_connect(ctx);
// 					// }
// // #else
// 					// int saved_errno = errno;
// // 
// 					// if (errno == ETIMEDOUT)
// 					// {
// 						// _sleep_response_timeout(ctx);
// 						// modbus_flush(ctx);
// 					// }
// 					// else if (errno == EBADF)
// 					// {
// 						// modbus_close(ctx);
// 						// modbus_connect(ctx);
// 					// }
// 					// errno = saved_errno;
// // #endif
// 				// }
// 				// return -1;
// 			// }
// // 
// 			// rc = ctx->backend->recv(ctx, msg + msg_length, length_to_read);
// 			// if (rc == 0)
// 			// {
// 				// errno = ECONNRESET;
// 				// rc = -1;
// 			// }
// // 
// 			// if (rc == -1)
// 			// {
// 				// _error_print(ctx, "read");
// 				// if ((ctx->error_recovery & MODBUS_ERROR_RECOVERY_LINK) &&
// 					//  (errno == ECONNRESET || errno == ECONNREFUSED || errno == EBADF))
// 				// {
// 					// int saved_errno = errno;
// 					// modbus_close(ctx);
// 					// modbus_connect(ctx);
// 					// /* Could be removed by previous calls */
// 					// errno = saved_errno;
// 				// }
// // #endif
// 				// return -1;
// 			// }
// // 
// 			// /* Display the hex code of each character received */
// 			// if (ctx->debug)
// 			// {
// 				// int i;
// 				// for (i = 0; i < rc; i++)
// 					// printf("<%.2X>", msg[msg_length + i]);
// 			// }
// // 
// 			// if (ctx->log)
// 			// {
// 				// int i;
// 				// for (i = 0; i < rc; i++)
// 				// {
// 					// ret += sprintf(ctx->rx_log_buffer + ret, "<%.2X>", msg[msg_length + i]);
// 				// }
// 				sprintf(ctx->rx_log_buffer + ret, "\n");
// // 
// 				sprintf(ctx->log_buffer + ret, "\n");
// 			// }
// // 
// 			// /* Sums bytes received */
// 			// msg_length += rc;
// 			// /* Computes remaining bytes */
// 			// length_to_read -= rc;
// // 
// 			// if (length_to_read == 0)
// 			// {
// 				// switch (step)
// 				// {
// 				// case _STEP_FUNCTION:
// 					// /* Function code position */
// 					// length_to_read = compute_meta_length_after_function(
// 						//  msg[ctx->backend->header_length], msg_type);
// 					// if (length_to_read != 0)
// 					// {
// 						// step = _STEP_META;
// 						// break;
// 					// } /* else switches straight to the next step */
// 				// case _STEP_META:
// 					// length_to_read = compute_data_length_after_meta(ctx, msg, msg_type);
// 					// if ((msg_length + length_to_read) > ctx->backend->max_adu_length)
// 					// {
// 						// errno = EMBBADDATA;
// 						// _error_print(ctx, "too many data");
// 						// return -1;
// 					// }
// 					// step = _STEP_DATA;
// 					// break;
// 				// default:
// 					// break;
// 				// }
// 			// }
// // 
// 			// if (length_to_read > 0 &&
// 				//  (ctx->byte_timeout.tv_sec > 0 || ctx->byte_timeout.tv_usec > 0))
// 			// {
// 				// /* If there is no character in the buffer, the allowed timeout
// 					// interval between two consecutive bytes is defined by
// 					// byte_timeout */
// 				// tv.tv_sec = ctx->byte_timeout.tv_sec;
// 				// tv.tv_usec = ctx->byte_timeout.tv_usec;
// 				// p_tv = &tv;
// 			// }
// 			// /* else timeout isn't set again, the full response must be read before
// 				// expiration of response timeout (for CONFIRMATION only) */
// 		// }
// // 
// 		// if (ctx->debug)
// 			// printf("\n");
// // 
// 		if (ctx->log) {
// 		    sprintf(ctx->rx_log_buffer, "\n");
// 		}
// // 
// 		// return ctx->backend->check_integrity(ctx, msg, msg_length);
// 	// }
// // 
// 	void setRts(bool flag) {
// 		int flags;
// 		ioctl(m_fd, TIOCMGET, &flags);
// 		if (flag) flags |= TIOCM_RTS;
// 		else flags &= ~TIOCM_RTS;
// 		ioctl(m_fd, TIOCMSET, &flags);
// 	}
// // 
// // private:
// 	// int m_fd;
// // 
// // }
// // 
// #if HAVE_DECL_TIOCM_RTS
// 	static void _serial_ioctl_rts(modbus_t *ctx, int on)
// 	{
// 		int fd = ctx->s;
// 		int flags;
// // 
// 		ioctl(fd, TIOCMGET, &flags);
// 		if (on)
// 		{
// 			flags |= TIOCM_RTS;
// 		}
// 		else
// 		{
// 			flags &= ~TIOCM_RTS;
// 		}
// 		ioctl(fd, TIOCMSET, &flags);
// 	}
// #endif
// // 
// 	static ssize_t serial_send(modbus_t *ctx, const uint8_t *req, int req_length)
// 	{
// 	#if HAVE_DECL_TIOCM_RTS
// 		modbus_rtu_t *ctx_rtu = ctx->backend_data;
// 		if (ctx_rtu->rts != MODBUS_RTU_RTS_NONE)
// 		{
// 			ssize_t size;
// // 
// 			if (ctx->debug)
// 			{
// 				fprintf(stderr, "Sending request using RTS signal\n");
// 			}
// // 
// // 			ctx_rtu->set_rts(ctx, ctx_rtu->rts == MODBUS_RTU_RTS_UP);
// // 			usleep(ctx_rtu->rts_delay);

// // 			size = write(ctx->s, req, req_length);

// // 			usleep(ctx_rtu->onebyte_time * req_length + ctx_rtu->rts_delay);
// // 			ctx_rtu->set_rts(ctx, ctx_rtu->rts != MODBUS_RTU_RTS_UP);

// // 			return size;
// // 		}
// // 		else
// // 		{
// // #endif
// // 			return write(ctx->s, req, req_length);
// // #if HAVE_DECL_TIOCM_RTS
// // 		}
// // #endif
// // #endif
// // 	}

}
}