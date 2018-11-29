#ifndef MQTT_h
#define MQTT_h

#include "Arduino.h"
#include "Message.h"
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

class MQTT {
  public:
    MQTT();
    void loop();
    void setServer(char* server);
  private:
    void _checkConnection();
    bool _isReconnecting;
    char _server[30];
    WiFiClient _espClient;
    PubSubClient _mqtt; 
    void _callback(char* topic, byte* payload, unsigned int length);
};

#endif
