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
	Sensor.setup();
	Sensor.setMeasurement(10, measure); //produce a measurement value every X seconds
}

void loop() {
	Sensor.loop();
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
