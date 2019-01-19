#ifndef Bus_h
#define Bus_h

#include "Arduino.h"
#include <CAN.h>

#define CAN_MAX_PACKET_SIZE 1024

typedef void (*CANMessageCallback)(char* topic, byte* payload, unsigned int length);

class Bus {
public:
	Bus();
	void set_message_callback(CANMessageCallback callback) { this->message_callback = callback; }
	void setup();
	void loop();
	byte packet[CAN_MAX_PACKET_SIZE];
	unsigned int packetLength;
	unsigned int topicLength;
	unsigned int payloadLength;
	bool isValid;
	void send(const byte* topic, unsigned int topicLength, const byte* payload, unsigned int payloadLength);

	static void publish(const char* topic, const uint8_t* payload, unsigned int payload_length);

private:
	void _callback(int length);

private:
	CANMessageCallback message_callback = 0;
	bool _ready;
};

extern Bus bus;

#endif
