#include "Sensor.h"

#ifdef DEVICE_SENSOR_TEMPHUM
	#include <DHT.h>
#endif

#ifdef DEVICE_SENSOR_TEMPHUM
	DHT dht(5, DHT11);
#endif

void setup() {
	sensor.setup();
	sensor.setMeasurement(10, measure); //produce a measurement value every X seconds
}

void loop() {
	sensor.loop();
}

void measure() {
	#ifdef DEVICE_SENSOR_BRIGHTNESS_LOWRES
		sensor.measured(1, "brightness", ((double)(1023-analogRead(0))/1023.0)*100.0, "%");
	#endif

	#ifdef DEVICE_SENSOR_BRIGHTNESS_HIGHRES
		sensor.measured(2, "brightness", ((double)analogRead(0)/4095.0)*100.0, "%");
	#endif

	#ifdef DEVICE_SENSOR_TEMPHUM
		sensor.measured(3, "temperature", dht.readTemperature(), "°C");
		sensor.measured(4, "humidity", dht.readHumidity(), "%");
	#endif
}
