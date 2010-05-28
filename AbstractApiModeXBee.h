#ifndef AbstractApiModeXBee_h
#define AbstractApiModeXBee_h

#include "NewSoftSerial/NewSoftSerial.h"
#include "LowlevelXBee.h"
#include <inttypes.h>

/**
 * Low-Level interface to an API-mode XBee attached serially.
 * The communication is based upon packets that can be sent to or received from the locally attached xbee. This
 * interface does not provide native support for communication with remote xbees, you have to encode data to send
 * into the appropriate packets yourself.
 *
 * An implementation of this class handles:
 * 	- calculation of checksums
 * 	- setting/encoding of the packet's length
 *  - buffering
 *  - composing and error-checking the received packets
 */
class AbstractApiModeXBee : public LowlevelXBee {
public:
	/**
	 * Sends the payload as a packet to the xbee.
	 * The length of the payload must be <= 252 bytes
	 * @param payload payload to send
	 * @param length length of the payload
	 */
	void send(uint8_t *payload, uint8_t length);

	/**
	 * @return true if a packet is available
	 */
	bool available();

	/**
	 * Receive the next packet from the xbee.
	 *
	 * @param data will be set to a pointer to payload of the packet. Will only be
	 * 		valid until the next call to receive or available.
	 * @param length will be set to the length of data
	 * @return true if a packet was available
	 */
	bool receive(uint8_t **data, uint8_t *length);

protected:
	void init();
	virtual bool serialAvailable();
	virtual uint8_t serialRead();
	virtual void serialPrint(uint8_t data);

private:
	uint8_t _buffer[MAX_PACKET_SIZE];
	uint8_t _buffer_length;
	uint8_t _payload_length;

	void sendByte(uint8_t data);
	bool parseInput();
	bool skipToNextPacket();
};


#endif
