#include "udp/UdpCom.h"

void UdpCom::start() {
    sockpp::socket_initializer::initialize();
    sock = sockpp::udp_socket();
    if (!sock || !sock.bind(sockpp::inet_address("0.0.0.0", receivePort))) {
        initialized = false;
        return;
    }

    if (!sock.set_non_blocking(true)) {
        initialized = false;
        return;
    }

    destAddr = sockpp::inet_address(destIp, destPort);
    ComTask::packetRateMonitor.reset();
    ComTask::motorBoardComStateMonitor.reset();
    ComTask::fillingStationBoardComStateMonitor.reset();

    initialized = true;
}

bool UdpCom::read() {
    if (!initialized || !sock.is_open()) {
        return false;
    }
    
    char incomingDataBuffer[incomingDataBufferSize];
    sockpp::inet_address senderAddr;
    bool receivedAtLeastOne = false;
    

    while (true) {
        
        SSIZE_T bytesReceived = sock.recv_from(incomingDataBuffer, sizeof(incomingDataBuffer), &senderAddr);
        
        if (bytesReceived <= 0) {
            break;
        }

        receivedAtLeastOne = true;
        
        for (int i = 0; i < bytesReceived; i++) {
            ComTask::udpPacketReceiver.receiveByte(incomingDataBuffer[i]);
        }
    }
    return receivedAtLeastOne;
}

bool UdpCom::write(std::span<const uint8_t> msg) {
    if (!initialized || !sock.is_open()) {
        return false;
    }

    SSIZE_T bytesSent = sock.send_to(msg.data(), msg.size(), destAddr);

    if (bytesSent <= 0) {
        return false;
    }

    return (static_cast<size_t>(bytesSent) == msg.size());
}

bool UdpCom::openConnectionTo(std::string ipAddress, uint16_t sendPort, uint16_t receivePort) {
    if (comOpened()) {
        shutdown();
    }
    
    this->destPort = sendPort;
    this->receivePort = receivePort;
    this->destIp = ipAddress;
    
    start();
    
    if (comOpened()) {
        return true;
    }
    return false;
}

bool UdpCom::comOpened() {
    return (initialized && sock.is_open());
}

void UdpCom::shutdown() {
    if (sock.is_open()) {
        sock.close();
    }

    initialized = false;
}

bool UdpCom::getPacket(uint8_t* recv) {
    return ComTask::udpPacketReceiver.getPacket(recv);
}

uint8_t* UdpCom::getBuffer() {
    return ComTask::udpPacketReceiver.getBuffer();
}

std::string UdpCom::getProtocolName() {
    return protocolNameFromEnum(comType);
}

std::optional<std::string> UdpCom::getConnectionDetails() {
    if (comOpened()) {
        std::string details;
        details = "Listening on " + destIp + ":" + std::to_string(receivePort);
        return details;
    }
    return std::nullopt;
}

ComType UdpCom::getComType() const {
    return comType;
}
