#include "MQTT.h"

MQTT::MQTT() : _mqtt(_espClient) {
  _mqtt.setCallback([this] (char* topic, byte* payload, unsigned int length) { this->_callback(topic, payload, length); });
}

void MQTT::loop() {
  _checkConnection();
}

void MQTT::_checkConnection() {
  if (_mqtt.connected() || _isReconnecting || strcmp(_server, "") == 0 || WiFi.status() != WL_CONNECTED) return;

  Serial.print("Attempting MQTT connection to ");
  Serial.print(_server);
  Serial.print(" with clientId ");
  String clientId = "Sensor-" + String(ESP.getChipId(), HEX);
  Serial.println(clientId);

  WiFi.mode(WIFI_STA);
  if (_mqtt.connect(clientId.c_str())) {
    Serial.println("connected");
  } else {
    Serial.print("failed, rc=");
    Serial.print(_mqtt.state());
    _isReconnecting = true;
    _reconnectTicker.once(10, _reconnectTimeout, this);
  }
}

void MQTT::setServer(char* server) {
  strcpy(_server, server);
  _mqtt.setServer(_server, 1883);
}

void MQTT::_reconnectTimeout(MQTT* mqtt) {
    mqtt->_isReconnecting = false;
}

void MQTT::_callback(char* topic, byte* payload, unsigned int length) {
  
}
