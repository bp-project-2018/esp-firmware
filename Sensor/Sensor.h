#ifndef Sensor_h
#define Sensor_h

#define DEVICE_BRIDGE
//#define DEVICE_SENSOR_TEMPHUM
//#define DEVICE_SENSOR_BRIGHTNESS

#include "Arduino.h"

#define WIFI_SSID "sysnet-bp"
#define WIFI_PASSWORD "totallysecure"
#define MQTT_SERVER "192.168.10.1"

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#endif
#ifdef ESP32
#include <WiFi.h>
#endif
#include <Ticker.h>
#include "MQTT.h"
#include <ArduinoJson.h>

class SensorClass {
  public:
  SensorClass();
  void loop();
  void setup();
  void setMeasurement(int interval, void (*callback)());
  void measured(char* type, double value, char* unit);
  char chipId[10];
  private:
  MQTT _mqtt;
  
  Ticker _measurementTicker;
  bool _willMeasure;
  void (*_measurementCallback)();
  static void _measureTick(SensorClass* sensor);
};

extern SensorClass Sensor;

#endif
