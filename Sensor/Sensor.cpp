#include "CommProto.h"

#include "Sensor.h"

void mqtt_connect_callback();
void mqtt_message_callback(char* topic, byte* payload, unsigned int length);
void bus_message_callback(char* topic, byte* payload, unsigned int length);
void datagram_callback(const char* address, const byte* data, int data_length);

Sensor sensor;

Sensor::Sensor() {
	#ifdef ESP8266
		snprintf(chipId, 10, "%04X", ESP.getChipId());
	#endif
	#ifdef ESP32
		snprintf(chipId, 10, "%04X", (uint16_t)(ESP.getEfuseMac()>>32));
	#endif

	Serial.begin(9600);
}

void Sensor::setup() {
	#if defined(ESP8266) || defined(DEVICE_BRIDGE)
		{
			// Set wifi hostname.
			char hostname[20];
			sprintf(hostname, "Sensor-%s", chipId);
			#ifdef ESP8266
				WiFi.hostname(hostname);
			#endif
			#ifdef ESP32
				WiFi.setHostname(hostname);
			#endif
		}

		WiFi.mode(WIFI_STA);
		WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
		while (WiFi.status() != WL_CONNECTED) {
			delay(500);
			Serial.print(".");
		}
		Serial.println("WiFi connected");
		Serial.println("IP address: ");
		Serial.println(WiFi.localIP());

		#ifdef ESP8266
			{
				// Set OTA hostname.
				char hostname[20];
				sprintf(hostname, "bp-sensor-%s", sensor.chipId);
				ArduinoOTA.setHostname(hostname);
				ArduinoOTA.begin();
			}
		#endif

		mqtt.setServer(MQTT_SERVER);

		mqtt.set_connect_callback(mqtt_connect_callback);
		mqtt.set_message_callback(mqtt_message_callback);

		protocol.setup(MQTT::publish);
	#endif

	#ifdef ESP32
		bus.setup();
		bus.set_message_callback(bus_message_callback);

		#ifndef DEVICE_BRIDGE
			// Set up protocol via CAN bus.
			protocol.setup(Bus::publish);
			protocol.on_transport_connect(NULL);
		#endif
	#endif

	protocol.set_datagram_callback(datagram_callback);
}

void Sensor::loop() {
	#if defined(ESP8266) || defined(DEVICE_BRIDGE)
		mqtt.loop();
	#endif
	#ifdef ESP32
		bus.loop();
	#endif
	if(_willMeasure && _measurementCallback) { // millis will eventually overflow
		_willMeasure = false;
		Serial.println("Collecting measurements");
		_measurementCallback();
	}
}

void Sensor::setMeasurement(int interval, void (*callback)()) {
	_measurementCallback = callback;
	_measurementTicker.attach(interval, Sensor::_measureTick, this);
}

void Sensor::_measureTick(Sensor* sensor) {
	sensor->_willMeasure = true;
}

void Sensor::measured(int id, char* type, double value, char* unit) {
	if(isnan(value)) {
		Serial.print("Measurement failed of ");
		Serial.print(type);
		Serial.println(" failed");
		return;
	}
	
	Serial.print("Measured ");
	Serial.print(type);
	Serial.print(" of ");
	Serial.print(value);
	Serial.print(" ");
	Serial.println(unit);

	StaticJsonDocument<512> doc;
	JsonObject root = doc.to<JsonObject>();
	root["device_id"] = chipId;
	root["sensor_id"] = id;
	root["value"] = value;
	root["type"] = type;
	root["unit"] = unit;

	char json[512];
	serializeJson(doc, json);

	protocol.send("kronos", (const byte*) json, strlen(json));
}

void mqtt_connect_callback() {
	#ifdef DEVICE_BRIDGE
		MQTT::subscribe("#");
		protocol.on_transport_connect(NULL);
	#else
		protocol.on_transport_connect(MQTT::subscribe);
	#endif
}

void mqtt_message_callback(char* topic, byte* payload, unsigned int length) {
	Serial.print("Received mqtt message on ");
	Serial.print(topic);
	Serial.print(" (");
	Serial.print(length, DEC);
	Serial.println(" bytes)");

	#ifdef DEVICE_BRIDGE
		Bus::publish(topic, payload, length);
	#endif

	protocol.on_transport_message(topic, payload, length);
}

void bus_message_callback(char* topic, byte* payload, unsigned int length) {
	Serial.print("Received bus message on ");
	Serial.print(topic);
	Serial.print(" (");
	Serial.print(length, DEC);
	Serial.println(" bytes)");

	#ifdef DEVICE_BRIDGE
		MQTT::publish(topic, payload, length);
	#endif

	protocol.on_transport_message(topic, payload, length);
}

void datagram_callback(const char* address, const byte* data, int data_length) {
	Serial.print("Received datagram from ");
	Serial.print(address);
	Serial.print(": ");
	Serial.println((const char*) data);

	if (strcmp((const char*) data, "ping") == 0) {
		const char* message = "pong";
		protocol.send(address, (const byte*) message, strlen(message));
	}
}
