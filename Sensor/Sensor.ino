#include "MQTT.h"
#include "CommProto.h"
#include "Sensor.h"

#ifdef DEVICE_SENSOR_TEMPHUM
#define HAS_TEMPHUM_DHT
#endif

#ifdef DEVICE_SENSOR_BRIGHTNESS
#define HAS_BRIGHTNESS_ADC
#endif


#ifdef HAS_TEMPHUM_DHT
#include <DHT.h>
DHT dht(5, DHT11);
#endif

void setup() {
  mqtt.set_callback(mqtt_callback);
 
  protocol.set_mqtt_send_func([](const char* topic, const uint8_t* payload, unsigned int payload_length){
    mqtt.pubSub.publish(topic, payload, payload_length);
  });

  Sensor.setup();
  mqtt.setServer(MQTT_SERVER);
  Sensor.setMeasurement(10, measure); //produce a measurement value every X seconds

  #ifdef ESP8266
  char hostname[20];
  sprintf(hostname, "bp-sensor-%s", Sensor.chipId);
  ArduinoOTA.setHostname(hostname);
  ArduinoOTA.begin();
  #endif
}

void loop() {
  mqtt.loop();
  Sensor.loop();
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Received message on ");
  Serial.print(topic);
  Serial.print(" (");
  Serial.print(length, DEC);
  Serial.println(" bytes)");

  protocol.on_mqtt_message(topic, payload, length);
}

void measure() {
  #ifdef HAS_BRIGHTNESS_ADC
  Sensor.measured("brightness", ((double)(1024-analogRead(0))/(double)1024)*100, "%");
  #endif

  #ifdef HAS_TEMPHUM_DHT
  Sensor.measured("temperature",  dht.readTemperature(), "°C");
  Sensor.measured("humidity", dht.readHumidity(), "%");
  #endif
}
