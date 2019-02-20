#ifdef ESP32

#include "Bus.h"

Bus bus;

Bus::Bus() {}

void Bus::setup() {
	CAN.setPins(35, 5);

	// start the CAN bus at 500 kbps
	if (!CAN.begin(500E3)) {
		Serial.println("Starting CAN-Controller failed");
	}

	CAN.onReceive([](int length) { bus.callback(length); });
}

void Bus::loop() {
	while (receive_buffers[ready_buffer_index].status == ReceiveBufferStatus::READY) {
		ReceiveBuffer& buffer = receive_buffers[ready_buffer_index];
		char* topic = (char*) (buffer.data);
		byte* payload = (byte*) (topic+buffer.topic_length);
		if (message_callback) message_callback(topic, payload, buffer.payload_length);
		buffer.status = ReceiveBufferStatus::EMPTY;
		if (++ready_buffer_index == CAN_RECEIVE_BUFFER_COUNT) ready_buffer_index = 0;
	}
}

void Bus::callback(int length) {
	if (CAN.packetRtr()) return; // ignore transmission request

	ReceiveBuffer& buffer = receive_buffers[current_buffer_index];

	switch (CAN.packetId()) {
	case 0: // start of transmission

		if (buffer.status == ReceiveBufferStatus::READY) {
			// Previous packet has not yet been processed by the loop().
			// Ignore new packet.
			Serial.println("Dropping CAN packet");
			return;
		}

		if (buffer.status == ReceiveBufferStatus::RECEIVING) {
			// Another packet was already in progress.
			// Discard old partial packet and start again.
			Serial.println("CAN transmission interrupted by new packet");
		}

		buffer.status = ReceiveBufferStatus::RECEIVING;

		buffer.topic_length = (((unsigned int)(CAN.read())) << 8) + ((unsigned int)(CAN.read()));
		buffer.payload_length = (((unsigned int)(CAN.read())) << 8) + ((unsigned int)(CAN.read()));
		buffer.received_length = 0;
		break;

	case 1: // end of transmission
	case 2: // data packet

		if (buffer.status != ReceiveBufferStatus::RECEIVING) return;

		if (buffer.received_length + CAN.available() > CAN_MAX_PACKET_SIZE) {
			// Received packet too large. Drop it.
			Serial.println("Received CAN packet too large");
			buffer.status = ReceiveBufferStatus::EMPTY;
			return;
		}

		while (CAN.available()) buffer.data[buffer.received_length++] = CAN.read();

		if (CAN.packetId() == 1) { // end of transmission
			if (buffer.received_length == buffer.topic_length + buffer.payload_length) {
				buffer.data[buffer.topic_length-1] = 0;
				buffer.data[buffer.received_length] = 0;
				// Mark transmission as successful.
				buffer.status = ReceiveBufferStatus::READY;
				if (++current_buffer_index == CAN_RECEIVE_BUFFER_COUNT) current_buffer_index = 0;
			} else {
				Serial.println("CAN receive failed");
				buffer.status = ReceiveBufferStatus::EMPTY;
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
	for(int i = 0; receive_buffers[current_buffer_index].status == ReceiveBufferStatus::RECEIVING && i < 20; i++) {
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
