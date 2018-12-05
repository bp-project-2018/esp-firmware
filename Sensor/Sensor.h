#ifndef Sensor_h
#define Sensor_h

#include "Arduino.h"
#include <Crypto.h>

#define WIFI_SSID "sysnet-bp"
#define WIFI_PASSWORD "totallysecure"
#define MQTT_SERVER "192.168.10.1"
#define AES_KEY_LENGTH 16
const byte AES_KEY[AES_KEY_LENGTH] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
const SHA256HMAC HMAC(AES_KEY, AES_KEY_LENGTH);

#include <ESP8266WiFi.h>
#include <Ticker.h>
#include "MQTT.h"
#include <ArduinoJson.h>

class SensorClass {
  public:
  SensorClass();
  void loop();
  void setMeasurement(int interval, void (*callback)());
  void measured(char* type, double value, char* unit);
  private:
  MQTT _mqtt;
  
  Ticker _measurementTicker;
  bool _willMeasure;
  void (*_measurementCallback)();
  static void _measureTick(SensorClass* sensor);
};

extern SensorClass Sensor;

#endif
