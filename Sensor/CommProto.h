#ifndef CommProto_h
#define CommProto_h

#include <Ticker.h>

#include "Arduino.h"
#include "Datagram.h"
#include "CommProtoConfig.h"

#define COMMPROTO_MAX_DATAGRAM_SIZE 1024

typedef void (*DatagramCallback)(const char* address, const byte* data, int data_length);

typedef void (*TransportPublishFunc)(const char* topic, const uint8_t* payload, unsigned int payload_length);
typedef void (*TransportSubscribeFunc)(const char* topic);

class CommProto {
public:
	// API to connect with the Arduino Sketch and the Transport (MQTT/CAN Bus/etc.).

	// Must be called during setup to initialize the instance.
	void setup(TransportPublishFunc publish);
	// Must be called after connecting to the transport server to (re)subscribe to channels.
	void on_transport_connect(TransportSubscribeFunc subscribe);
	// Must be called to notify the protocol of incoming messages.
	void on_transport_message(char* topic, byte* payload, unsigned int length);

private:
	static void time_request_callback(CommProto* self);
	void send_time_request();

public:
	// Public API to interact with the communication protocol.
	void send(const char* address, const byte* data, int data_len);
	void set_datagram_callback(DatagramCallback callback) { this->callback = callback; }

private:
	const PartnerConfig* find_partner(const char* address);
	int64_t get_current_time();

private:
	TransportPublishFunc publish = 0;
	DatagramCallback callback = 0;

private:
	Ticker time_request_ticker;

	int64_t last_timestamps[partner_configurations_length];

	bool last_valid = false;
	byte last_nonce[DATAGRAM_NONCE_SIZE];
	unsigned long last_millis = 0;

	// @Todo: Millis will overflow after approximately 50 days.
    int64_t timestamp = 0; // Official timestamp from the server.
    unsigned long timestamp_millis = 0; // Local time when the timestamp was received to calculate delta.
};

// Singleton instance.
extern CommProto protocol;

#endif
