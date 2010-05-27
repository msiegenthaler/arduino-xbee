#include "WConstants.h"
#include "Series1XBee.h"
#include <WProgram.h>

bool isBroadcastAddress(XBeeAddress address) {
	return address == BROADCAST_ADDRESS;
}


Series1XBee::Series1XBee(LowlevelXBee *lowlevel) {
	_lowlevel = lowlevel;
	_length = 0;
}

void Series1XBee::send(XBeeAddress address, uint8_t *data, uint8_t length) {
	uint8_t len = length+11;
	uint8_t buffer[len];
	buffer[0] = 0x00; //Api-Id
	buffer[1] = 0x00; //Frame-Id
	buffer[2] = (address >> 56) & 0xFF;
	buffer[3] = (address >> 48) & 0xFF;
	buffer[4] = (address >> 40) & 0xFF;
	buffer[5] = (address >> 32) & 0xFF;
	buffer[6] = (address >> 24) & 0xFF;
	buffer[7] = (address >> 16) & 0xFF;
	buffer[8] = (address >> 8) & 0xFF;
	buffer[9] = address & 0xFF;
	buffer[10] = 0x00; //Options
	memcpy(&buffer+11, data, length);
	_lowlevel->send(buffer, len);
}

bool Series1XBee::available() {
	if (_length > 0) return true;

	return skipToNextRx();
}

bool Series1XBee::receive(XBeeAddress *from, uint8_t **data, uint8_t *length) {
	if (_length == 0) {
		if (!skipToNextRx()) return false;
	}

	if (_data[0] == 0x80) {
		(*from) = 0;
		(*from) |= ((XBeeAddress)_data[1]) << 56;
		(*from) |= ((XBeeAddress)_data[2]) << 48;
		(*from) |= ((XBeeAddress)_data[3]) << 40;
		(*from) |= ((XBeeAddress)_data[4]) << 32;
		(*from) |= ((XBeeAddress)_data[5]) << 24;
		(*from) |= ((XBeeAddress)_data[6]) << 16;
		(*from) |= ((XBeeAddress)_data[7]) <<  8;
		(*from) |= _data[8];
		//_data[9]  Signal Strength
		//_data[10] Options

		(*length) = _length - 11;
		(*data) = ((uint8_t*)_data) + 11;
		_length = 0;
		return true;
	} else if (_data[0] == 0x81) {
		(*from) = 0;
		(*from) |= ((XBeeAddress)_data[1]) << 8;
		(*from) |= _data[2];
		//_data[3]  Signal Strength
		//_data[4]  Options

		(*length) = _length - 5;
		(*data) = ((uint8_t*)_data) + 5;
		_length = 0;
		return true;
	} else {
		// Unknown
		_length = 0;
		return false;
	}
}

bool Series1XBee::skipToNextRx() {
	_length = 0;
	while (true) {
		uint8_t len;
		if (!_lowlevel->receive(&_data, &len))
			return false;

		//Only care for RX packets
		if (_data[0] == 0x80) {
			//64 bit rx
			_length = len;
			return true;
		} else if (_data[0] == 0x81) {
			//16 bit rx
			_length = len;
			return true;
		} else {
			//not relevant message
		}
	}
}
