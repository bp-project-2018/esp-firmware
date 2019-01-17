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

class CommProto {
public:
	// Public API to interact with the communication protocol.
	void send(const char* address, const byte* data, int data_len);
	void set_datagram_callback(DatagramCallback callback) { this->callback = callback; }

private:
	const PartnerConfig* find_partner(const char* address);

public:
	// Must be called to notify the protocol of incoming MQTT messages.
	void on_mqtt_message(char* topic, byte* payload, unsigned int length);
	// Must be called to be able to send messages.
	void set_mqtt_send_func(MQTTPublishFunc publish) { this->publish = publish; }

private:
	DatagramCallback callback = 0;
	MQTTPublishFunc publish = 0;
};

// Singleton instance.
extern CommProto protocol;

#endif
