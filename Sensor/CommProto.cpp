#include "CommProto.h"
#include "CommProtoConfig.h"

// Singleton instance.
CommProto protocol;

void CommProto::send(const char* address, const byte* data, int data_length) {
	const PartnerConfig* receiver_config = find_partner(address);
	if (!receiver_config) {
		Serial.print("CommProto::send: address not found: ");
		Serial.println(address);
		return;
	}

	int64_t timestamp = 0;
	// timestamp, err := client.getTime()
	// if err != nil {
	// 	return fmt.Errorf("failed to get time: %v", err)
	// }

	byte iv[DATAGRAM_IV_SIZE];
	generate_random_bytes(iv, sizeof(iv));

	byte buffer[COMMPROTO_MAX_DATAGRAM_SIZE];
	int length = assemble_datagram(buffer, sizeof(buffer), host_address, iv, timestamp, data, data_length, receiver_config->key, receiver_config->passphrase);
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

void CommProto::on_mqtt_message(char* topic, byte* payload, unsigned int length) {
	

}
