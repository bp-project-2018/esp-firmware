#ifndef Sensor_h
#define Sensor_h

#define WIFI_SSID "sysnet-bp"
#define WIFI_PASSWORD "totallysecure"
#define MQTT_SERVER "10.0.0.1"

#include "MQTT.h"
#include <ESP8266WiFi.h>

class Sensor {
  public:
  Sensor();
  void loop();
  private:
  MQTT _mqtt;
};

#endif
