#pragma once

#include "CircularBuffer.hpp"
#include "SerialConfig.h"

using PacketCircularBuffer = CircularBuffer<SerialConfig::PACKET_CIRCULAR_BUFFER_SIZE>;
