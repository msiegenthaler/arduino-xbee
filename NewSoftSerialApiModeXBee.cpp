#include "WConstants.h"
#include "NewSoftSerialApiModeXBee.h"
#include <WProgram.h>

NewSoftSerialApiModeXBee::NewSoftSerialApiModeXBee(NewSoftSerial *serial) {
	_serial = serial;
	init();
}

bool NewSoftSerialApiModeXBee::serialAvailable() {
	return _serial->available();
}

uint8_t NewSoftSerialApiModeXBee::serialRead() {
	return _serial->read();
}

void NewSoftSerialApiModeXBee::serialPrint(uint8_t data) {
	_serial->print(data);
}
