#include "Sensor.h"

#ifdef DEVICE_SENSOR_TEMPHUM
	#ifdef ESP32
		#include <DHTesp.h>
	#else
		#include <DHT.h>
	#endif
#endif

#ifdef DEVICE_SENSOR_TEMPHUM
	#ifdef ESP32
		DHTesp dht;
	#else
		DHT dht(5, DHT11);
	#endif
#endif

void setup() {
	#if defined(DEVICE_SENSOR_TEMPHUM) && defined(ESP32)
		dht.setup(16, DHTesp::DHT11);
	#endif
	sensor.setup();
	sensor.setMeasurement(10, measure); //produce a measurement value every X seconds
}

void loop() {
	sensor.loop();
}

void measure() {
	#ifdef DEVICE_SENSOR_BRIGHTNESS
	{
		double brightness;
		#ifdef ESP32
			brightness = ((double)analogRead(0)/4095.0)*100.0;
		#else
			brightness = ((double)(1023-analogRead(0))/1023.0)*100.0;
		#endif
		sensor.measured(1, "brightness", brightness, "%");
	}
	#endif

	#ifdef DEVICE_SENSOR_TEMPHUM
	{
		double temperature, humidity;
		#ifdef ESP32
			auto values = dht.getTempAndHumidity();
			temperature = values.temperature;
			humidity = values.humidity;
		#else
			temperature = dht.readTemperature();
			humidity = dht.readHumidity();
		#endif
		sensor.measured(2, "temperature", temperature, "°C");
		sensor.measured(3, "humidity", humidity, "%");
	}
	#endif
}
