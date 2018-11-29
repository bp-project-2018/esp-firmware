#include "Sensor.h"

Sensor::Sensor() : _mqtt() {
    WiFi.hostname("Sensor-" + ESP.getChipId());
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    _mqtt.setServer(MQTT_SERVER);
}

void Sensor::loop() {
    _mqtt.loop();
}
