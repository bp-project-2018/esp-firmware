#include "CommProto.h"

// Singleton instance.
CommProto protocol;

void CommProto::setup(TransportPublishFunc publish) {
	this->publish = publish;
}

void CommProto::on_transport_connect(TransportSubscribeFunc subscribe) {
	char topic[256];

	snprintf(topic, sizeof(topic), "%s/inbox", host_address);
	if (subscribe) subscribe(topic);

	snprintf(topic, sizeof(topic), "%s/time", host_address);
	if (subscribe) subscribe(topic);

	if (!timestamp) {
		// Send initial request.
		send_time_request();

		// Repeat sending requests until timestamp is received.
		time_request_ticker.attach(1, time_request_callback, this);
	}
}

void CommProto::on_transport_message(char* topic, byte* message, unsigned int message_length) {
	char expected[256];	

	snprintf(expected, sizeof(expected), "%s/inbox", host_address);
	if (strcmp(topic, expected) == 0) {
		// Handle datagram.

		char address[256];
		if (!extract_address(message, message_length, address)) return;

		const PartnerConfig* sender = find_partner(address);
		if (!sender) return;

		int64_t timestamp;
		byte data[COMMPROTO_MAX_DATAGRAM_SIZE+1];
		int data_length;
		if (!disassemble_datagram(message, message_length, address, sender->key, sender->passphrase, &timestamp, data, sizeof(data), &data_length)) return;

		int64_t current = get_current_time();
		if (!current) return;

		int64_t delta = timestamp - current;
		if (delta < -1000000000 /* ns */ || delta > 1000000000 /* ns */) return; // @Hardcoded

		int sender_index = sender - partner_configurations;
		int64_t last = last_timestamps[sender_index];
		if (last != 0 && timestamp <= last) return;
		last_timestamps[sender_index] = timestamp;

		// Make payload zero-terminated for convenience.
		data[data_length] = 0;

		if (callback) callback(address, data, data_length);
		return;
	}

	snprintf(expected, sizeof(expected), "%s/time", host_address);
	if (strcmp(topic, expected) == 0) {
		// Handle time response.

		char address[256];
		if (!extract_address(message, message_length, address)) return;
		if (strcmp(address, time_server_config->address) != 0) return;

		int64_t timestamp;
		byte nonce[DATAGRAM_NONCE_SIZE];
		if (!disassemble_time_response(message, message_length, address, time_server_config->passphrase, &timestamp, nonce)) return;

		if (!last_valid) return;
		if (memcmp(nonce, last_nonce, DATAGRAM_NONCE_SIZE) != 0) return;
		last_valid = false;
		if (millis() - last_millis > 1000) return;

		this->timestamp = timestamp;
		this->timestamp_millis = millis();

		time_request_ticker.detach();
		return;
	}
}

void CommProto::time_request_callback(CommProto* self) {
	self->send_time_request();
}

void CommProto::send_time_request() {
	generate_random_bytes(last_nonce, DATAGRAM_NONCE_SIZE);

	byte request[COMMPROTO_MAX_DATAGRAM_SIZE+1];
	int length = assemble_time_request(request, sizeof(request), host_address, last_nonce, time_server_config->passphrase);
	if (!length) return;

	last_millis = millis();
	last_valid = true;

	char topic[256];
	snprintf(topic, sizeof(topic), "%s/time/request", time_server_config->address);
	if (publish) publish(topic, request, length);
	Serial.println("Sent time request");
}

void CommProto::send(const char* address, const byte* data, int data_length) {
	const PartnerConfig* receiver_config = find_partner(address);
	if (!receiver_config) {
		Serial.print("CommProto::send: address not found: ");
		Serial.println(address);
		return;
	}

	int64_t current_time = get_current_time();
	if (!current_time) {
		Serial.println("CommProto::send: no timestamp received yet");
		return;
	}

	byte iv[DATAGRAM_IV_SIZE];
	generate_random_bytes(iv, sizeof(iv));

	byte buffer[COMMPROTO_MAX_DATAGRAM_SIZE];
	int length = assemble_datagram(buffer, sizeof(buffer), host_address, iv, current_time, data, data_length, receiver_config->key, receiver_config->passphrase);
	if (!length) {
		Serial.println("CommProto::send: failed to assemble datagram");
		return;
	}

	char topic[256];
	snprintf(topic, sizeof(topic), "%s/inbox", address);
	if (publish) publish(topic, buffer, length);
}

const PartnerConfig* CommProto::find_partner(const char* address) {
	for (int i = 0; i < partner_configurations_length; i++) {
		if (strcmp(address, partner_configurations[i].address) == 0) {
			return &partner_configurations[i];
		}
	}
	return nullptr;
}

int64_t CommProto::get_current_time() {
	if (!timestamp) return 0;
	unsigned long delta = millis() - timestamp_millis;
	return timestamp + int64_t(delta) * 1e6;
}
