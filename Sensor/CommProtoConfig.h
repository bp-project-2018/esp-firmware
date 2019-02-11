#ifndef CommProtoConfig_h
#define CommProtoConfig_h

#include "Datagram.h"

struct PartnerConfig {
	const char* address;
	const char* passphrase;
	const byte key[DATAGRAM_KEY_SIZE];
};

const char* const host_address = "kalliope";

const PartnerConfig partner_configurations[] = {
	{
		"kronos",
		"Ich hätte gerne einen Whopper.",
		{ 0xa9, 0xda, 0x59, 0x62, 0xeb, 0xa6, 0xce, 0x63, 0xef, 0xb6, 0xed, 0xa1, 0x8c, 0xae, 0x21, 0x23 }
	},
	{
		"shredder",
		"There are 69,105 leaves in a pile.",
		{ 0x04, 0x80, 0x1c, 0xe1, 0x6b, 0x94, 0x5a, 0xb0, 0x59, 0x86, 0xdc, 0xf9, 0x4d, 0xc8, 0x2c, 0x2f }
	}
};

const int partner_configurations_length = sizeof(partner_configurations) / sizeof(partner_configurations[0]);

const PartnerConfig* const time_server_config = &partner_configurations[0];

#endif
