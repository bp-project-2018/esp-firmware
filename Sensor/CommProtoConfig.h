#ifndef CommProtoConfig_h
#define CommProtoConfig_h

#include "SensorConfig.h"
#include "Datagram.h"

struct PartnerConfig {
	const char* address;
	const char* passphrase;
	const byte key[DATAGRAM_KEY_SIZE];
};

// NOTE: The first partner_configuration is always used as the time server right now.
// See declaration of time_server_config below.

#if SENSOR_CONFIG == SENSOR_KALLIOPE
	const char* const host_address = "kalliope";
	const PartnerConfig partner_configurations[] = {
		{ "kronos",   "Ich haette gerne einen Whopper.",    { 0xa9, 0xda, 0x59, 0x62, 0xeb, 0xa6, 0xce, 0x63, 0xef, 0xb6, 0xed, 0xa1, 0x8c, 0xae, 0x21, 0x23 } },
		{ "shredder", "There are 69,105 leaves in a pile.", { 0x04, 0x80, 0x1c, 0xe1, 0x6b, 0x94, 0x5a, 0xb0, 0x59, 0x86, 0xdc, 0xf9, 0x4d, 0xc8, 0x2c, 0x2f } }
	};

#elif SENSOR_CONFIG == SENSOR_JULIAMUELLER
	const char* const host_address = "juliamueller";
	const PartnerConfig partner_configurations[] = {
		{ "kronos", "Die Kuh die lacht.", { 0xdd, 0x37, 0x3c, 0x07, 0x27, 0x54, 0xb3, 0xa8, 0xd6, 0xc7, 0xe3, 0xd8, 0xf6, 0x0c, 0xbf, 0x5a } }
	}; 

#elif SENSOR_CONFIG == SENSOR_MAX
	const char* const host_address = "max";
	const PartnerConfig partner_configurations[] = {
		{ "kronos", "max@germany.de", { 0x3b, 0x0e, 0x5c, 0x2d, 0xf5, 0xc8, 0x0e, 0x86, 0x16, 0xd7, 0x25, 0x5b, 0xbe, 0x76, 0x19, 0x9e } }
	};

#elif SENSOR_CONFIG == SENSOR_PETER
	const char* const host_address = "peter";
	const PartnerConfig partner_configurations[] = {
		{ "kronos", "Peter Arbeitsloser.", { 0x14, 0x15, 0xe1, 0xd4, 0xac, 0x16, 0x06, 0x30, 0xc8, 0xeb, 0x9f, 0x63, 0xde, 0xff, 0xaf, 0xbb } }
	};

#elif SENSOR_CONFIG == SENSOR_MARCUWE
	const char* const host_address = "marcuwe";
	const PartnerConfig partner_configurations[] = {
		{ "kronos", "Parkwaechter grillen.", { 0xa4, 0xbc, 0x60, 0xa8, 0xba, 0xcb, 0x81, 0x7a, 0xfe, 0x29, 0x03, 0xd1, 0xdd, 0xad, 0x2f, 0xcc } }
	};

#else
	#error No network configuration for SENSOR_CONFIG.
#endif

const int partner_configurations_length = sizeof(partner_configurations) / sizeof(partner_configurations[0]);

const PartnerConfig* const time_server_config = &partner_configurations[0];

#endif
