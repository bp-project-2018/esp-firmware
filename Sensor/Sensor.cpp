#include "Sensor.h"

Sensor::Sensor() : _mqtt() {
    Serial.begin(9600);
    WiFi.hostname("Sensor-" + ESP.getChipId());
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    _mqtt.setServer(MQTT_SERVER);
}

void Sensor::loop() {
    _mqtt.loop();
}

void Sensor::setMeasurement(int interval, void (*callback)()) {
    _measureTicker.attach(interval, callback);
}

void Sensor::measured(char* type, double value, char* unit) {
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
