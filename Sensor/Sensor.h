#ifndef Sensor_h
#define Sensor_h

#include "SensorConfig.h"
#include "Arduino.h"

#ifdef ESP8266
  #include <ESP8266WiFi.h>
  #include <ArduinoOTA.h>
#endif

#ifdef ESP32
  #include <WiFi.h>
  #include "Bus.h"
#endif

#include <Ticker.h>
#include "MQTT.h"
#include <ArduinoJson.h>

class Sensor {
public:
	Sensor();
	void loop();
	void setup();
	void setMeasurement(int interval, void (*callback)());
	void measured(int id, char* type, double value, char* unit);
	char chipId[10];

private:
	Ticker _measurementTicker;
	bool _willMeasure;
	void (*_measurementCallback)();
	static void _measureTick(Sensor* sensor);
};

extern Sensor sensor;

#endif
