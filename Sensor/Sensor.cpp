#include "CommProto.h"

#include "Sensor.h"

SensorClass Sensor;

SensorClass::SensorClass() {
    #ifdef ESP8266
    snprintf(chipId, 10, "%04X", ESP.getChipId());
    #endif
    #ifdef ESP32
    snprintf(chipId, 10, "%04X", (uint16_t)(ESP.getEfuseMac()>>32));
    #endif

    Serial.begin(9600);
}

void SensorClass::setup() {
	#if defined(ESP8266) || defined(DEVICE_BRIDGE)

    char hostname[20];
    sprintf(hostname, "Sensor-%s", chipId);
    #ifdef ESP8266
    WiFi.hostname(hostname);
    #endif
    #ifdef ESP32
    WiFi.setHostname(hostname);
    #endif
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    #ifdef ESP8266
	char hostname[20];
	sprintf(hostname, "bp-sensor-%s", Sensor.chipId);
	ArduinoOTA.setHostname(hostname);
	ArduinoOTA.begin();
	#endif

	mqtt.setServer(MQTT_SERVER);

	protocol.set_mqtt_send_func([](const char* topic, const uint8_t* payload, unsigned int payload_length){
	    mqtt.pubSub.publish(topic, payload, payload_length);
	});

	#endif

	#ifdef ESP32
    bus.setup();
    #endif
}

void SensorClass::loop() {
	mqtt.loop();
	bus.loop();
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

    StaticJsonDocument<512> doc;
    JsonObject root = doc.to<JsonObject>();
    root["device_id"] = chipId;
    root["sensor_id"] = 1;
    root["value"] = value;
    root["type"] = type;
    root["unit"] = unit;

    char json[512];
    serializeJson(doc, json);

    protocol.send("shredder", (const byte*) json, strlen(json));
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Received message on ");
  Serial.print(topic);
  Serial.print(" (");
  Serial.print(length, DEC);
  Serial.println(" bytes)");

  protocol.on_mqtt_message(topic, payload, length);
}
