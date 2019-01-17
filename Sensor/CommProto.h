#ifndef CommProto_h
#define CommProto_h

#include "Arduino.h"
#include "Datagram.h"

#define COMMPROTO_MAX_DATAGRAM_SIZE 1024

struct PartnerConfig {
	const char* address;
	const char* passphrase;
	const byte key[DATAGRAM_KEY_SIZE];
};

typedef void (*DatagramCallback)(const char* address, const byte* data, int data_length);

typedef void (*MQTTPublishFunc)(const char* topic, const uint8_t* payload, unsigned int payload_length);
typedef void (*MQTTSubscribeFunc)(const char* topic);

class CommProto {
public:
	// API to connect with MQTT and Arduino.

	// Must be called during setup to initialize the instance.
	void setup(MQTTPublishFunc publish);
	// Must be called after connecting to the MQTT server to subscribe to channels.
	void on_mqtt_connect(MQTTSubscribeFunc subscribe);
	// Must be called to notify the protocol of incoming MQTT messages.
	void on_mqtt_message(char* topic, byte* payload, unsigned int length);

private:
	static void time_request_callback(CommProto* self);

public:
	// Public API to interact with the communication protocol.
	void send(const char* address, const byte* data, int data_len);
	void set_datagram_callback(DatagramCallback callback) { this->callback = callback; }

private:
	const PartnerConfig* find_partner(const char* address);

private:
	MQTTPublishFunc publish = 0;
	DatagramCallback callback = 0;
};

// Singleton instance.
extern CommProto protocol;

#endif
