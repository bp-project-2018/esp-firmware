#ifndef Bus_h
#define Bus_h

#include "Arduino.h"
#include <CAN.h>

#define CAN_MAX_PACKET_SIZE 1024
#define CAN_RECEIVE_BUFFER_COUNT 2

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
	enum class ReceiveBufferStatus {
		EMPTY, RECEIVING, READY
	};

	struct ReceiveBuffer {
		ReceiveBufferStatus status = ReceiveBufferStatus::EMPTY;
		unsigned int topic_length, payload_length, received_length;
		byte data[CAN_MAX_PACKET_SIZE+1];
	};

private:
	CANMessageCallback message_callback = 0;
	int ready_buffer_index = 0;
	int current_buffer_index = 0;
	ReceiveBuffer receive_buffers[CAN_RECEIVE_BUFFER_COUNT];
};

extern Bus bus;

#endif
