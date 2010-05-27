#ifndef Series1XBee_h
#define Series1XBee_h

#import "LowlevelXBee.h"

#define WPAN_MAX_PAYLOAD 100
#define XBeeAddress uint64_t

#define BROADCAST_ADDRESS 0xFFFF
bool isBroadcastAddress(XBeeAddress address);

/**
 * Sends/receives message using the series 1 xbee protocol.
 * All messages are sent untracked.
 */
class Series1XBee {
public:
	/**
	 * Constructor
	 */
	Series1XBee(LowlevelXBee *lowlevel);

	/**
	 * Send a wpan data packet.
	 * @param to receipent of the packet
	 * @param data data to send
	 * @param length length of data. Must be <= 100
	 */
	void send(XBeeAddress to, uint8_t *data, uint8_t length);

	/**
	 * @return true if a data packet is available.
	 */
	bool available();

	/**
	 * Get the next message received by this xbee.
	 * @param from will be set to the sender of the message
	 * @param data will be set to the payload of the message. Will only be valid until the
	 * 		next call to available or receive.
	 * @param length will be set to the length of data
	 * @return true if a message was received
	 */
	bool receive(XBeeAddress *from, uint8_t **data, uint8_t *length);

private:
	LowlevelXBee *_lowlevel;

	uint8_t *_data;
	uint8_t _length;

	bool skipToNextRx();
};

#endif
