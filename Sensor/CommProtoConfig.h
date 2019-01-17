#ifndef CommProtoConfig_h
#define CommProtoConfig_h

#include "CommProto.h"

const char* const host_address = "kronos";

const PartnerConfig partner_configurations[] = {
	{
		"shredder",
		"Ein Idiot in Uniform ist immer noch ein Idiot.",
		{ 0xe8, 0xc4, 0xe4, 0x83, 0x0a, 0x9c, 0xfc, 0x1b, 0x05, 0xbe, 0xd9, 0xa2, 0xf5, 0x59, 0x08, 0x10 }
	}
};

const int partner_configurations_length = sizeof(partner_configurations) / sizeof(partner_configurations[0]);

const PartnerConfig* const time_server_config = &partner_configurations[0];

#endif
