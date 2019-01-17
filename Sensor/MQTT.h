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

class MQTT {
  public:
    MQTT();
    void loop();
    void setServer(char* server);
    void set_callback(void (*callback)(char* topic, byte* payload, unsigned int length));

    PubSubClient pubSub;

  private:
    void _checkConnection();
    bool _isReconnecting;
    char _server[30];
    WiFiClient _espClient;
    Ticker _reconnectTicker;
    static void _reconnectTimeout(MQTT* mqtt);
};

extern MQTT mqtt;

#endif
