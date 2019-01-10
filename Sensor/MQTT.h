#ifndef MQTT_h
#define MQTT_h

#include "Arduino.h"
#include "Datagram.h"

#include "PubSubClient.h"

#include <Ticker.h>

#ifdef ESP8266
    #include <WiFiClient.h>
    #include <ESP8266WiFi.h>
#endif

#ifdef ESP32
    #include <WiFi.h>
#endif

class MQTT {
  public:
    MQTT();
    void loop();
    void setServer(char* server);

    // @Todo: Millis will overflow after approximately 50 days.
    int32_t timestamp = 0; // Official timestamp from the server.
    unsigned long timestamp_millis = 0; // Local time when the timestamp was received to calculate delta.

    Datagram datagram;
    PubSubClient _mqtt; //dirty, make private
  private:
    void _checkConnection();
    bool _isReconnecting;
    char _server[30];
    WiFiClient _espClient;
    Ticker _reconnectTicker;
    static void _reconnectTimeout(MQTT* mqtt);
    void _callback(char* topic, byte* payload, unsigned int length);

    Ticker _timeRequestTicker;
    static void _timeRequestCallback(MQTT* mqtt);
};

#endif
