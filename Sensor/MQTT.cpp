#include "MQTT.h"
#include "Sensor.h"

MQTT mqtt;

MQTT::MQTT() : pubSub(_espClient) {}

void MQTT::set_callback(void (*callback)(char* topic, byte* payload, unsigned int length)) {
  pubSub.setCallback(callback);
}

void MQTT::loop() {
  _checkConnection();
  pubSub.loop();
}

void MQTT::_checkConnection() {
  if (pubSub.connected() || _isReconnecting || strcmp(_server, "") == 0 || WiFi.status() != WL_CONNECTED) return;

  Serial.print("Attempting MQTT connection to ");
  Serial.print(_server);
  Serial.print(" with clientId ");
  String clientId = "Sensor-" + String((char*)Sensor.chipId);
  Serial.println(clientId);

  WiFi.mode(WIFI_STA);
  if (pubSub.connect(clientId.c_str())) {
    Serial.println("connected");
  } else {
    Serial.print("failed, rc=");
    Serial.print(pubSub.state());
    _isReconnecting = true;
    _reconnectTicker.once(10, _reconnectTimeout, this);
  }
}

void MQTT::setServer(char* server) {
  strcpy(_server, server);
  pubSub.setServer(_server, 1883);
}

void MQTT::_reconnectTimeout(MQTT* mqtt) {
    mqtt->_isReconnecting = false;
}
