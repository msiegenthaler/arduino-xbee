#ifndef LowlevelXBee_h
#define LowlevelXBee_h

#undef int
#undef abs
#undef double
#undef float
#undef round

#include "WProgram.h"
#include <inttypes.h>

#define DEBUG_LOWLEVEL
//#define DEBUG_LOWLEVEL to get serial debug output for lowlevel packets (api-commands)
#define DEBUG_PACKETS
//#define DEBUG_PACKETS to get serial debug output for all packets sent and received to/from other xbees

#ifdef DEBUG_LOWLEVEL
#define DEBUG_FUNS
#endif
#ifdef DEBUG_PACKETS
#define DEBUG_FUNS
#endif
#ifdef DEBUG_FUNS
void printHex(uint8_t *data, uint8_t len, bool seperate);
void printAddress(uint64_t address);
#endif



#define MAX_PACKET_SIZE 256


/**
 * Low-Level interface to the directly attached xbee.
 * The communication is based upon packets that can be sent to or received from the locally attached xbee. This
 * interface does not provide native support for communication with remote xbees, you have to encode data to send
 * into the appropriate packets yourself.
 *
 * An implementation of handles:
 * 	- calculation of checksums
 * 	- setting/encoding of the packet's length
 *  - buffering
 *  - composing and error-checking the received packets
 */
class LowlevelXBee {
public:
	/**
	 * Sends the payload as a packet to the xbee.
	 * The length of the payload must be <= 252 bytes
	 * @param payload payload to send
	 * @param length length of the payload
	 */
	virtual void send(uint8_t *payload, uint8_t length);

	/**
	 * @return true if a packet is available
	 */
	virtual bool available();

	/**
	 * Receive the next packet from the xbee.
	 *
	 * @param data will be set to a pointer to payload of the packet. Will only be
	 * 		valid until the next call to receive or available.
	 * @param length will be set to the length of data
	 * @return true if a packet was available
	 */
	virtual bool receive(uint8_t **data, uint8_t *length);
};


#endif
