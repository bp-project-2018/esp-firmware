#include "MQTT.h"
#include "Sensor.h"
#include "Datagram.h"

MQTT::MQTT() : _mqtt(_espClient) {
  _mqtt.setCallback([this] (char* topic, byte* payload, unsigned int length) { this->_callback(topic, payload, length); });
}

void MQTT::loop() {
  _checkConnection();
  _mqtt.loop();
}

void MQTT::_checkConnection() {
  if (_mqtt.connected() || _isReconnecting || strcmp(_server, "") == 0 || WiFi.status() != WL_CONNECTED) return;

  Serial.print("Attempting MQTT connection to ");
  Serial.print(_server);
  Serial.print(" with clientId ");
  String clientId = "Sensor-" + String((char*)Sensor.chipId);
  Serial.println(clientId);

  WiFi.mode(WIFI_STA);
  if (_mqtt.connect(clientId.c_str())) {
    Serial.println("connected");
    {
      char topic[256];

      // Subscribe to time.
      snprintf(topic, 256, "%s/time", TIME_SERVER_ADDRESS);
      _mqtt.subscribe(topic);

      if (!timestamp) {
        // Send initial request.
        snprintf(topic, 256, "%s/time/request", TIME_SERVER_ADDRESS);
        _mqtt.publish(topic, "");

        // Repeat sending requests until timestamp is received.
        _timeRequestTicker.attach(1, _timeRequestCallback, this);
      }
    }
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

void MQTT::_timeRequestCallback(MQTT* mqtt) {
  char topic[256];
  snprintf(topic, 256, "%s/time/request", TIME_SERVER_ADDRESS);
  mqtt->_mqtt.publish(topic, "");
}

void MQTT::_callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Received message on ");
  Serial.print(topic);
  Serial.print(" (");
  Serial.print(length, DEC);
  Serial.println(" bytes)");
  char expected[256];
  snprintf(expected, 256, "%s/time", TIME_SERVER_ADDRESS);
  if (strcmp(topic, expected) == 0) {
    if (decode_time(payload, length, &timestamp)) {
      timestamp_millis = millis();
      _timeRequestTicker.detach();
    }
  }
}
