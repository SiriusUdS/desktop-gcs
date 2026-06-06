#pragma once

#include "ICom.h"

std::string ICom::protocolNameFromEnum(ComType comType) {
	switch (comType) {
		case ComType::SERIAL:
			return "Serial";
		case ComType::UDP:
			return "UDP";
		default:
			return "Unknown";
	}
}
