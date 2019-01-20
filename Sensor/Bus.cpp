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

    packet[CAN_MAX_PACKET_SIZE+1] = '\0'; //terminate buffer for safety reasons
    
    char* topic = (char*)packet+2;
		unsigned int topicLength = strlen(topic);
   
    byte* payload = (byte*)topic+topicLength+1;
   
		payloadLength = int(packet[0]) << 8;
		payloadLength += int(packet[1]);

		Serial.print("Recevied topic length: ");
		Serial.print(topicLength);
		Serial.print(", payload length: ");
		Serial.print(payloadLength);
		Serial.print(", expected packet length: ");
		Serial.print(2 + topicLength + 1 + payloadLength);
		Serial.print(", received packet length: ");
		Serial.println(packetLength);

		if(packetLength == 2 + topicLength + 1 + payloadLength) { //valid transmission
			Serial.println("Valid CAN transmission received");
			message_callback(topic, payload, payloadLength);
		} else {
			Serial.println("Invalid CAN transmission.");
		}
	}
}

void Bus::send(const char* topic, const byte* payload, unsigned int payload_length) {
  if(2 + strlen(topic) + 1 + payload_length > CAN_MAX_PACKET_SIZE) return; //messages too large
  
	for(int i = 0; !_ready && i < 20; i++) { //bus taken, wait until free again but maximum 20ms
		delay(1);
	}

  {
    packetLength = 0;
  	packet[packetLength++] = byte((payload_length & 0xff00) >> 8);
  	packet[packetLength++] = byte(payload_length & 0xff);
  }

  {
    strncpy((char*)packet+packetLength, topic, 255);
    packetLength += strlen(topic);
    packet[packetLength++] = '\0';
  }

  {
  	memcpy(packet+packetLength, payload, payload_length);
    packetLength += payload_length;
  }

  int bytes = 0;
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
	bus.send(topic, payload, payload_length);
}

#endif
