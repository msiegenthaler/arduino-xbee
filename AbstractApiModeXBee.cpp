#include "WConstants.h"
#include "AbstractApiModeXBee.h"
#include <WProgram.h>

#define XBEE_START_BYTE 0x7E
#define PAYLOAD_OFFSET 3

#ifdef DEBUG_FUNS
void printHex(uint8_t *data, uint8_t len, bool seperate) {
	for (int i=0; i<len; i++) {
		if (i>0 && seperate) Serial.print(" ");
		if (data[i] < 0x10) Serial.print("0");
		Serial.print(data[i], HEX);
	}
}
void printAddress(uint64_t address) {
	printHex((uint8_t*)&address, 8, false);
}
#endif

void AbstractApiModeXBee::init() {
	_buffer_length = 0;
	_payload_length = 0;
}


void AbstractApiModeXBee::send(uint8_t *payload, uint8_t length) {
	sendByte(XBEE_START_BYTE);
	sendByte(0); //more significant bytes of length --> always 0
	sendByte(length);

	uint8_t checksum = 0;
	for (uint8_t i=0; i<length; i++) {
		sendByte(payload[i]);
		checksum += payload[i];
	}
	checksum = 0xFF - checksum;
	sendByte(checksum);

#ifdef DEBUG_LOWLEVEL
	Serial.print("Lowlevel XBee packet out '");
	printHex(payload, length, true);
	Serial.println("'");
#endif
}

bool AbstractApiModeXBee::available() {
	return parseInput();
}

bool AbstractApiModeXBee::receive(uint8_t **data, uint8_t *length) {
	parseInput();
	if (_payload_length == 0)
		return false;

	(*data) = ((uint8_t*)_buffer) + PAYLOAD_OFFSET;
	(*length) = _payload_length;
	_payload_length = 0;

#ifdef DEBUG_LOWLEVEL
	Serial.print("Lowlevel XBee packet in '");
	printHex(*data, *length, true);
	Serial.println("'");
#endif
	return true;
}

bool AbstractApiModeXBee::parseInput() {
	if (_payload_length > 0) return true;
	if (!serialAvailable()) return false;

	if (_buffer_length == 0) {
		//No data yet, search for the beginning of the next packet
		if (!skipToNextPacket())
			return false;
	}

	//read the length of the packet
	while (_buffer_length < PAYLOAD_OFFSET && serialAvailable()) {
		_buffer[_buffer_length] = serialRead();
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
	while (_buffer_length < packetLength && serialAvailable()) {
		_buffer[_buffer_length] = serialRead();
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
#ifdef DEBUG_LOWLEVEL
	Serial.println("Discarded xbee packet because checksum was wrong.");
#endif
		return false;
	}

	//everything ok, we got us a packet
	_payload_length = payloadLength;
	_buffer_length = 0;
	return true;
}

bool AbstractApiModeXBee::skipToNextPacket() {
	while (serialAvailable()) {
		uint8_t b = serialRead();
		if (b == XBEE_START_BYTE) {
			_buffer[0] = b;
			_buffer_length = 1;
			return true;
		}
	}
	return false;
}

void AbstractApiModeXBee::sendByte(uint8_t data) {
	serialPrint(data);
}

bool AbstractApiModeXBee::serialAvailable() { return false; }
uint8_t AbstractApiModeXBee::serialRead() { return 0; }
void AbstractApiModeXBee::serialPrint(uint8_t data) {}


