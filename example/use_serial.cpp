#include <iostream>

#include "Serial.h"

int main(void) {
	mb::interaction::Serial serial("/tmp/ttyV0", 19200, 'N', 8, 1);
	serial.connect();
	uint8_t query[] = {0x01, 0x01, 0x00, 0x01, 0x00, 0x03, 0x2D, 0xCB};
	serial.send(query,8);


	mb::interaction::Serial serialV1("/tmp/ttyV1", 19200, 'N', 8, 1);
	serialV1.connect();
	uint8_t resp[128] = {0};
	// serialV1.waitReceive(resp,128);
	
	// serial.closeSocket();
	// serialV1.closeSocket();

	for (int i = 0; i < 8; i++) {
		printf("0x%02X ", resp[i]);
	}
	printf("\n");

	return 0;
}