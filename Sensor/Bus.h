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
	void callback(int length);

private:
	CANMessageCallback message_callback = 0;
	int status;
	unsigned int topic_length, payload_length;
	unsigned int received_length;
	byte received_data[CAN_MAX_PACKET_SIZE+1];
};

extern Bus bus;

#endif
