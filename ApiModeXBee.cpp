#include "WConstants.h"
#include "ApiModeXBee.h"
#include <WProgram.h>

#define XBEE_START_BYTE 0x7E
#define PAYLOAD_OFFSET 3

ApiModeXBee::ApiModeXBee(NewSoftSerial *serial) {
	_serial = serial;
	_buffer_length = 0;
	_payload_length = 0;
}

void ApiModeXBee::send(uint8_t *payload, uint8_t length) {
	sendByte(XBEE_START_BYTE);
	sendByte(0); //more significant bytes of length --> always 0
	sendByte(length);
	_serial->flush();

	uint8_t checksum = 0;
	for (uint8_t i=0; i<length; i++) {
		sendByte(payload[i]);
		checksum += payload[i];
	}
	checksum = 0xFF - checksum;
	sendByte(checksum);
}

bool ApiModeXBee::available() {
	return parseInput();
}

bool ApiModeXBee::receive(uint8_t **data, uint8_t *length) {
	parseInput();
	if (_payload_length == 0)
		return false;

	(*data) = ((uint8_t*)_buffer) + PAYLOAD_OFFSET;
	(*length) = _payload_length;
	_payload_length = 0;
	return true;
}

bool ApiModeXBee::parseInput() {
	if (_payload_length > 0) return true;
	if (!_serial->available()) return false;

	if (_buffer_length == 0) {
		//No data yet, search for the beginning of the next packet
		if (!skipToNextPacket())
			return false;
	}

	//read the length of the packet
	while (_buffer_length < PAYLOAD_OFFSET && _serial->available()) {
		_buffer[_buffer_length] = _serial->read();
		_buffer_length++;
	}
	if (_buffer_length < PAYLOAD_OFFSET) return false;
	uint8_t payloadLength = _buffer[2]; // only support lengths up to 256
	if (payloadLength < 3) {
		//invalid packet length, skip..
		_buffer_length = 0;
		return false;
	}
	uint8_t packetLength = payloadLength + PAYLOAD_OFFSET + 1; //(start + 2 len) + data + (checksum)

	//read until the end of the packet
	while (_buffer_length < packetLength && _serial->available()) {
		_buffer[_buffer_length] = _serial->read();
		_buffer_length++;
	}
	if (_buffer_length < packetLength) return false;

	//we got the whole packet.. check it
	uint8_t checksum = 0;
	for (uint8_t i=0; i<payloadLength; i++) {
		checksum += _buffer[i+PAYLOAD_OFFSET];
	}
	checksum = 0xFF - checksum;
	if (checksum != _buffer[_buffer_length-1]) {
		//checksum mismatch
		_buffer_length = 0;
		return false;
	}

	//everything ok, we got us a packet
	_payload_length = payloadLength;
	_buffer_length = 0;
	return true;
}

bool ApiModeXBee::skipToNextPacket() {
	while (_serial->available()) {
		uint8_t b = _serial->read();
		if (b == XBEE_START_BYTE) {
			_buffer[0] = b;
			_buffer_length = 1;
			return true;
		}
	}
	return false;
}

void ApiModeXBee::sendByte(uint8_t data) {
	_serial->print(data);
}


