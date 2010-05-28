#ifndef HardSerialApiModeXBee_h
#define HardSerialApiModeXBee_h

#undef int
#undef abs
#undef double
#undef float
#undef round

#include "WProgram.h"
#include "NewSoftSerial/NewSoftSerial.h"
#include "AbstractApiModeXBee.h"

/**
 * Low-Level interface to an API-mode XBee attached serially (with a hardware serial port).
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
class HardSerialApiModeXBee : public AbstractApiModeXBee {
public:
	HardSerialApiModeXBee(HardwareSerial *serial);

protected:
	bool serialAvailable();
	uint8_t serialRead();
	void serialPrint(uint8_t data);

private:
	HardwareSerial *_serial;
};


#endif
