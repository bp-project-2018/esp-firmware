#ifndef Sensor_h
#define Sensor_h

#define WIFI_SSID "sysnet-bp"
#define WIFI_PASSWORD "totallysecure"
#define MQTT_SERVER "192.168.10.1"

#include "MQTT.h"
#include <ESP8266WiFi.h>
#include <Ticker.h>

class Sensor {
  public:
  Sensor();
  void loop();
  void setMeasurement(int interval, void (*callback)());
  void measured(char* type, double value, char* unit);
  private:
  MQTT _mqtt;
  Ticker _measureTicker;
};

#endif
