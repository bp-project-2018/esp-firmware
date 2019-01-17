#ifndef Bus_h
#define Bus_h

#include "Arduino.h"
#include <CAN.h>

#define CAN_MAX_PACKET_SIZE 1024

class Bus {
public:
	Bus();
	void loop();
	void setup();
	byte packet[CAN_MAX_PACKET_SIZE];
	unsigned int packetLength;
	unsigned int topicLength;
	unsigned int payloadLength;
	bool isValid;
	void send(byte* topic, unsigned int topicLength, byte* payload, unsigned int payloadLength);
private:
	void _callback(int length);
	bool _ready;
};

extern Bus bus;

#endif
