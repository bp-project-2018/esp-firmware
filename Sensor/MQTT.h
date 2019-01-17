#ifndef MQTT_h
#define MQTT_h

#include "Arduino.h"

#include "PubSubClient.h"

#include <Ticker.h>

#ifdef ESP8266
	#include <WiFiClient.h>
	#include <ESP8266WiFi.h>
#endif

#ifdef ESP32
	#include <WiFi.h>
#endif

typedef void (*MQTTConnectCallback)();
typedef void (*MQTTMessageCallback)(char* topic, byte* payload, unsigned int length);

class MQTT {
public:
	MQTT();
	void loop();
	void setServer(char* server);

	void set_connect_callback(MQTTConnectCallback callback) { this->connect_callback = callback; }
	void set_message_callback(MQTTMessageCallback callback) { pubSub.setCallback(callback); }

	static void publish(const char* topic, const uint8_t* payload, unsigned int payload_length);
	static void subscribe(const char* topic);

	PubSubClient pubSub;

private:
	void _checkConnection();
	bool _isReconnecting;
	char _server[30];
	MQTTConnectCallback connect_callback = 0;
	WiFiClient _espClient;
	Ticker _reconnectTicker;
	static void _reconnectTimeout(MQTT* mqtt);
};

extern MQTT mqtt;

#endif
