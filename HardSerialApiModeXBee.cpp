#include "WConstants.h"
#include "HardSerialApiModeXBee.h"
#include <WProgram.h>

HardSerialApiModeXBee::HardSerialApiModeXBee(HardwareSerial *serial) {
	_serial = serial;
	init();
}

bool HardSerialApiModeXBee::serialAvailable() {
	return _serial->available();
}

uint8_t HardSerialApiModeXBee::serialRead() {
	return _serial->read();
}

void HardSerialApiModeXBee::serialPrint(uint8_t data) {
	Serial.print(data, HEX);
	_serial->print(data);
}
