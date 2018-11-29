#define HAS_TEMPHUM_DHT
//#define HAS_BRIGHTNESS_ADC

#include "Sensor.h"

Sensor sensor;

#ifdef HAS_TEMPHUM_DHT
#include <DHT.h>
DHT dht(5, DHT11);
#endif

void setup() {
  sensor.setMeasurement(10, measure); //produce a measurement value every X seconds
}

void loop() {
  sensor.loop();
}

void measure() {
  #ifdef HAS_BRIGHTNESS_ADC
  sensor.measured("brightness", ((double)(1024-analogRead(0))/(double)1024)*100, "%");
  #endif

  #ifdef HAS_TEMPHUM_DHT
  sensor.measured("temperature",  dht.readTemperature(), "°C");
  sensor.measured("humidity", dht.readHumidity(), "%");
  #endif
}
