#include <iostream>
#include <memory>
#include <thread>

#include "IModbusCore.h"
#include "ModbusCoreRtu.h"
#include "ModbusConnection.h"
#include "Time.h"

void rtuCon2Func();

int main(void)
{
	mb::types::ModbusConnection rtu_con1;
	rtu_con1.response_timeout = 100;
	rtu_con1.byte_timeout = 100;
	rtu_con1.indication_timeout = 100;
	rtu_con1.delay_between_polls = 10;

	rtu_con1.type = mb::types::ModbusConnectionType::RTU;
	rtu_con1.rtu.serial_port = "/tmp/ttyV1";
	rtu_con1.rtu.baudrate = 19200;
	rtu_con1.rtu.parity = 'N';
	rtu_con1.rtu.data_bits = 8;
	rtu_con1.rtu.stop_bit = 1;

	std::thread con2(rtuCon2Func);
	con2.detach();

	std::unique_ptr<mb::core::IModbusCore> core1 = mb::core::IModbusCore::create(rtu_con1);

	core1->setDebug(true);
	if (!core1->connect()) std::cout << "error connect con1" << std::endl;

	// core.getRequestFromMaster();

	// uint8_t buf[128] = { 0 };
	// core->getRequestFromMaster(buf);
	// core->getResponseFromSlave();

	while(true) {
		core1->sendRequestToSlave();
		mb::helpers::sleep(1000);
	}
	core1->clearBuffer();

	return 0;
}

void rtuCon2Func() {
	mb::types::ModbusConnection rtu_con2;
	rtu_con2.response_timeout = 100;
	rtu_con2.byte_timeout = 100;
	rtu_con2.indication_timeout = 100;
	rtu_con2.delay_between_polls = 10;

	rtu_con2.type = mb::types::ModbusConnectionType::RTU;
	rtu_con2.rtu.serial_port = "/tmp/ttyV0";
	rtu_con2.rtu.baudrate = 19200;
	rtu_con2.rtu.parity = 'N';
	rtu_con2.rtu.data_bits = 8;
	rtu_con2.rtu.stop_bit = 1;
	std::unique_ptr<mb::core::IModbusCore> core2 = mb::core::IModbusCore::create(rtu_con2);

	core2->setDebug(true);
	if (!core2->connect())
		std::cout << "error connect con2" << std::endl;

	uint8_t msg[5] = {0};
	core2->getRequestFromMaster(msg);

	core2->clearBuffer();
}