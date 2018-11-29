#include "Sensor.h"

SensorClass::SensorClass() : _mqtt() {
    Serial.begin(9600);
    WiFi.hostname("Sensor-" + ESP.getChipId());
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    _mqtt.setServer(MQTT_SERVER);
}

void SensorClass::loop() {
    _mqtt.loop();
    if(_willMeasure && _measurementCallback) { //millis will eventually overflow
      _willMeasure = false;
      Serial.println("Collecting measurements");
      _measurementCallback();
    }
}

void SensorClass::setMeasurement(int interval, void (*callback)()) {
    _measurementCallback = callback;
    _measurementTicker.attach(interval, SensorClass::_measureTick, this);
}

void SensorClass::_measureTick(SensorClass* sensor) {
    sensor->_willMeasure = true;
}

void SensorClass::measured(char* type, double value, char* unit) {
    if(isnan(value)) {
      Serial.print("Measurement failed of ");
      Serial.print(type);
      Serial.println(" failed");
      return;
    }
    
    Serial.print("Measured ");
    Serial.print(type);
    Serial.print(" of ");
    Serial.print(value);
    Serial.print(" ");
    Serial.println(unit);
}

SensorClass Sensor;
