#ifdef ESP32

#include "Bus.h"

Bus bus;

Bus::Bus() {
	_ready = true;
}

void Bus::setup() {
	CAN.setPins(35, 5);
	// start the CAN bus at 500 kbps
	if (!CAN.begin(500E3)) {
		Serial.println("Starting CAN-Controller failed");
	}

	CAN.onReceive([](int length) { bus._callback(length); });
}

void Bus::loop() {



	// @Todo: Call callback like this:
	// char* topic = ...; // must be zero-terminated.
	// byte* payload = ...;
	// unsigned int length = ...;
	// if (message_callback) message_callback(topic, payload, length);
}

void Bus::_callback(int length) {
	if (CAN.packetRtr()) return; //we do not handle transmission requests for now

	if(CAN.packetId() == 0) { //start of transmission
		_ready = false;
		Serial.println("Someone is started to transmit a CAN packet");
		packetLength = 0;
		isValid = false;
	}

	if(packetLength+1 >= CAN_MAX_PACKET_SIZE) return;

	Serial.print("Received CAN frame with id 0x");
	Serial.print(CAN.packetId(), HEX);
	Serial.print(": ");
	while (CAN.available()) {
		byte b = CAN.read();
		packet[packetLength++] = b;
		Serial.print((char)b);
	}
	Serial.println();

	if(CAN.packetId() == 1) { //end of transmission
		_ready = true;
		Serial.println("CAN Bus free again");
		
		topicLength = int(packet[0]);
		payloadLength = int(packet[1]) << 8;
		payloadLength += int(packet[2]);

		Serial.print("Recevied topic length: ");
		Serial.print(topicLength);
		Serial.print(", payload length: ");
		Serial.print(payloadLength);
		Serial.print(", expected packet length: ");
		Serial.print(topicLength + payloadLength + 3);
		Serial.print(", received packet length: ");
		Serial.println(packetLength);

		if(packetLength == topicLength + payloadLength + 3) { //valid transmission
			Serial.println("Valid CAN transmission received");
			isValid = true;
		} else {
			Serial.println("Invalid CAN transmission.");
		}
	}
}

void Bus::send(const byte* topic, unsigned int topicLength, const byte* payload, unsigned int payloadLength) {
	for(int i = 0; !_ready && i < 20; i++) { //bus taken, wait until free again but maximum 20ms
		delay(1);
	}

	int bytes = 0;
	isValid = false;
	packet[0] = byte(topicLength);
	packet[1] = byte((payloadLength & 0xff00) >> 8);
	packet[2] = byte(payloadLength & 0xff);

	memcpy(packet+3, topic, topicLength);
	memcpy(packet+3+topicLength, payload, payloadLength);
	packetLength = topicLength + payloadLength + 3;

	for(unsigned int n = 0; n < packetLength; n++) {
		if(bytes == 0) {
			if(n == 0) { //first packet
				CAN.beginPacket(0);
			} else if(packetLength - n <= 8) { //signal last packet
				CAN.beginPacket(1);
			} else { //normal data packet
				CAN.beginPacket(2);
			}
		}

		CAN.write(packet[n]);

		bytes = (bytes+1)%8;
		if(bytes == 0 || n+1 == packetLength) { //last byte for this packet
			CAN.endPacket();
		}
	}

	_ready = true;
}

void Bus::publish(const char* topic, const uint8_t* payload, unsigned int payload_length) {
	const int topic_length = strlen(topic);
	bus.send((const byte*) topic, topic_length, payload, payload_length);
}

#endif
