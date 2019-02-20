#ifndef Bus_h
#define Bus_h

#include "Arduino.h"
#include <CAN.h>

#define CAN_MAX_PACKET_SIZE 1024

typedef void (*CANMessageCallback)(char* topic, byte* payload, unsigned int length);

class Bus {
public:
	Bus();

	void setup();
	void loop();

	void set_message_callback(CANMessageCallback callback) { this->message_callback = callback; }
	void send(const char* topic, const byte* payload, unsigned int payload_length);

	static void publish(const char* topic, const uint8_t* payload, unsigned int payload_length);

private:
	void _callback(int length);

private:
	byte packet[CAN_MAX_PACKET_SIZE+1];
	unsigned int packetLength;
	unsigned int payloadLength;
	bool _ready;

	byte finished_packet[CAN_MAX_PACKET_SIZE+1];
	unsigned int finished_packet_length = 0, finished_topic_length = 0, finished_payload_length = 0;

private:
	CANMessageCallback message_callback = 0;
};

extern Bus bus;

#endif
