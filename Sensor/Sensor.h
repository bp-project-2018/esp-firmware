#ifndef Sensor_h
#define Sensor_h

#define DEVICE_BRIDGE
//#define DEVICE_SENSOR_TEMPHUM
//#define DEVICE_SENSOR_BRIGHTNESS

#include "Arduino.h"
#include <Crypto.h>

#define WIFI_SSID "sysnet-bp"
#define WIFI_PASSWORD "totallysecure"
#define MQTT_SERVER "192.168.10.1"
#define AES_KEY_LENGTH 16
const byte AES_KEY[AES_KEY_LENGTH] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
const SHA256HMAC HMAC(AES_KEY, AES_KEY_LENGTH);

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
