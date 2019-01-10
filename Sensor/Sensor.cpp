#include "Sensor.h"

SensorClass::SensorClass() : _mqtt() {
    #ifdef ESP8266
    snprintf(chipId, 10, "%04X", ESP.getChipId());
    #endif
    #ifdef ESP32
    snprintf(chipId, 10, "%04X", (uint16_t)(ESP.getEfuseMac()>>32));
    #endif

    Serial.begin(9600);
}

void SensorClass::setup() {
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

    StaticJsonDocument<512> doc;
    JsonObject root = doc.to<JsonObject>();
    root["device_id"] = chipId;
    root["sensor_id"] = 1;
    root["value"] = value;
    root["type"] = type;
    root["unit"] = unit;

    char json[512];
    serializeJson(doc, json);

    Datagram datagram;
    datagram.type = Datagram::Datagram_Type::MESSAGE;
    datagram.version = 0;
    datagram.encoding = Datagram::Payload_Encoding::JSON;

    strcpy(datagram.address, "kronos");

    if (!_mqtt.timestamp) {
        Serial.println("No timestamp received yet");
        return;
    }

    { // Calculate current timestamp.
        unsigned long delta = (millis() - _mqtt.timestamp_millis) / 1000;
        int32_t timestamp = _mqtt.timestamp + delta;
        datagram.fixedLength = 4;
        datagram.fixed[0] = byte(timestamp >> 24);
        datagram.fixed[1] = byte(timestamp >> 16);
        datagram.fixed[2] = byte(timestamp >>  8);
        datagram.fixed[3] = byte(timestamp >>  0);
    }

    datagram.payload = (byte*) json;
    datagram.payloadLength = strlen(json);

    datagram.generate_random_iv();
    byte* result;
    int length;
    datagram.encode(&result, &length);
    if (!result) {
        Serial.println("Encoding datagram failed");
        return;
    }

    _mqtt._mqtt.publish("shredder/inbox", result, length, false);
    free(result);
    Serial.println("Ok!");
}

SensorClass Sensor;
