#pragma once

#include "UDPPacket.h"
#include "UDPDeviceCtrlFlags.h"
#include <cstdint>
#include <winsock2.h>

struct UdpPacketMetadata {
    //From original PacketMetadata
    enum class Status {NONE, VALID, INVALID, DUMP_IMMEDIATELY};
    
    Status status{Status::NONE};
    size_t size{};
    
    //UDP purposes
    std::uint32_t deviceID{};
    std::uint32_t payloadID{};
    std::uint32_t deviceTsMs{};
    
    UDPDeviceCtrlFlags deviceCtrlFlags{};
    std::uint8_t deviceState{};
    
    
    static UdpPacketMetadata fromNetworkFrame(const networking::FrameUDPPacketHeader& rawFrame, size_t verifiedPayloadSize) {
        UdpPacketMetadata cleanData;
        
        cleanData.status = Status::VALID;
        cleanData.size = verifiedPayloadSize;
        
        cleanData.deviceID = rawFrame.deviceId;
        cleanData.payloadID = rawFrame.payloadId;
        
        cleanData.deviceCtrlFlags = rawFrame.deviceCtrlFlags;
        cleanData.deviceState = rawFrame.deviceState;
        
        cleanData.deviceTsMs = ntohl(rawFrame.deviceTsMs);
        return cleanData;
    }
};