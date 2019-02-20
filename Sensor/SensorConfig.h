#ifndef SensorConfig_h
#define SensorConfig_h

#define WIFI_SSID "sysnet-bp"
#define WIFI_PASSWORD "totallysecure"
#define MQTT_SERVER "192.168.10.1"

// Available configurations.
#define SENSOR_KALLIOPE 1
#define SENSOR_JULIAMUELLER 2
#define SENSOR_MAX 3
#define SENSOR_PETER 4
#define SENSOR_MARCUWE 5

// Select one configuration from above.
#define SENSOR_CONFIG SENSOR_KALLIOPE

// What follows are the specific device configurations for the symbolic names above.
// Please note that the coresponding network configurations are stored in CommProtoConfig.h.
//
// Possible defines are:
//
// #define DEVICE_BRIDGE
// - The sensor acts as a MQTT-CAN-bridge (ESP32 only).
//
// #define DEVICE_SENSOR_BRIGHTNESS
// - The sensor has a brightness sensor.
//
// #define DEVICE_SENSOR_TEMPHUM
// - The sensor has a temperature and humidity sensor.
// - Connect a DHT11 sensor to pin 5 (ESP8266) or pin 16 (ESP32).
//
// NOTE: These defines are not mutually exclusive, i.e. a bridge can also have
// sensors and one ESP can have both a brightness and a temperature and humidity
// sensor. Just specify multiple defines.
//

#if SENSOR_CONFIG == SENSOR_KALLIOPE
	#define DEVICE_SENSOR_BRIGHTNESS

#elif SENSOR_CONFIG == SENSOR_JULIAMUELLER
	#define DEVICE_SENSOR_TEMPHUM

#elif SENSOR_CONFIG == SENSOR_MAX
	#define DEVICE_BRIDGE

#elif SENSOR_CONFIG == SENSOR_PETER
	#define DEVICE_SENSOR_BRIGHTNESS

#elif SENSOR_CONFIG == SENSOR_MARCUWE
	#define DEVICE_SENSOR_TEMPHUM

#else
	#error No device configuration for SENSOR_CONFIG.
#endif

// Bridge check.
#if defined(DEVICE_BRIDGE) && !defined(ESP32)
	#error DEVICE_BRIDGE only allowed on ESP32.
#endif

#endif
