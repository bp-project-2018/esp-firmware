#ifdef ESP32

#include "Bus.h"

#define STATUS_READY     0 // ready to receive packet
#define STATUS_RECEIVING 1 // packet transmission in progress
#define STATUS_RECEIVED  2 // packet ready for loop

Bus bus;

Bus::Bus() {
	status = STATUS_READY;
}

void Bus::setup() {
	CAN.setPins(35, 5);

	// start the CAN bus at 500 kbps
	if (!CAN.begin(500E3)) {
		Serial.println("Starting CAN-Controller failed");
	}

	CAN.onReceive([](int length) { bus.callback(length); });
}

void Bus::loop() {
	if (status == STATUS_RECEIVED) {
		char* topic = (char*) (received_data);
		byte* payload = (byte*) (topic+topic_length);
		if (message_callback) message_callback(topic, payload, payload_length);
		status = STATUS_READY;
	}
}

void Bus::callback(int length) {
	if (CAN.packetRtr()) return; // ignore transmission request

	switch (CAN.packetId()) {
	case 0: // start of transmission

		if (status == STATUS_RECEIVED) {
			// Previous packet has not yet been processed by the loop().
			// Ignore new packet.
			Serial.println("Dropping CAN packet");
			return;
		}

		if (status == STATUS_RECEIVING) {
			// Another packet was already in progress.
			// Discard old partial packet and start again.
			Serial.println("CAN transmission interrupted by new packet");
		}

		status = STATUS_RECEIVING;

		topic_length = (((unsigned int)(CAN.read())) << 8) + ((unsigned int)(CAN.read()));
		payload_length = (((unsigned int)(CAN.read())) << 8) + ((unsigned int)(CAN.read()));
		received_length = 0;
		break;

	case 1: // end of transmission
	case 2: // data packet

		if (status != STATUS_RECEIVING) return;

		if (received_length + CAN.available() > CAN_MAX_PACKET_SIZE) {
			// Received packet too large. Drop it.
			Serial.println("Received CAN packet too large");
			status = STATUS_READY;
			return;
		}

		while (CAN.available()) received_data[received_length++] = CAN.read();

		if (CAN.packetId() == 1) { // end of transmission
			if (received_length == topic_length + payload_length) {
				received_data[topic_length-1] = 0;
				received_data[received_length] = 0;
				// Mark transmission as successful.
				status = STATUS_RECEIVED;
			} else {
				Serial.println("CAN receive failed");
				status = STATUS_READY;
			}
		}
		break;
	}
}

void Bus::send(const char* topic, const byte* payload, unsigned int payload_length) {
	unsigned int topic_strlen = strlen(topic);
	unsigned int topic_length = topic_strlen + 1;
	unsigned int total_length = topic_length + payload_length;
	if (total_length > CAN_MAX_PACKET_SIZE) {
		Serial.println("Cannot send CAN packet: too large");
		return;
	}

	// bus taken, wait until free again but maximum 20ms
	for(int i = 0; status == STATUS_RECEIVING && i < 20; i++) {
		delay(1);
	}

	byte buffer[CAN_MAX_PACKET_SIZE];
	memcpy(buffer, topic, topic_length);
	memcpy(buffer + topic_length, payload, payload_length);

	CAN.beginPacket(0);
	CAN.write(byte(topic_length >> 8));
	CAN.write(byte(topic_length >> 0));
	CAN.write(byte(payload_length >> 8));
	CAN.write(byte(payload_length >> 0));
	CAN.endPacket();

	unsigned int n = 0;
	for (; n + 8 < total_length; n += 8) {
		CAN.beginPacket(2);
		CAN.write(buffer + n, 8);
		CAN.endPacket();
	}

	CAN.beginPacket(1);
	CAN.write(buffer + n, total_length - n);
	CAN.endPacket();
}

void Bus::publish(const char* topic, const uint8_t* payload, unsigned int payload_length) {
	bus.send(topic, payload, payload_length);
}

#endif
