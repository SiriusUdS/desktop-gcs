#pragma once
#include "udp/UdpCom.h"
#include <iostream>

void UdpCom::start() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        initialized = false;
        return;
    }

    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) {
        initialized = false;
        WSACleanup();
        return;
    }

    u_long mode = 1;
    ioctlsocket(sock, FIONBIO, &mode);

    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(destPort);
    inet_pton(AF_INET, destIp.c_str(), &destAddr.sin_addr);
    
    sockaddr_in receiveAddr = {};
    receiveAddr.sin_family = AF_INET;
    receiveAddr.sin_addr.s_addr = INADDR_ANY;
    receiveAddr.sin_port = htons(receivePort);
    if (bind(sock, (struct sockaddr*) &receiveAddr, sizeof(receiveAddr)) == SOCKET_ERROR) {
        int err = WSAGetLastError();
        closesocket(sock);
        sock = INVALID_SOCKET;
        return;
    }

    SerialTask::packetRateMonitor.reset();
    SerialTask::motorBoardComStateMonitor.reset();
    SerialTask::fillingStationBoardComStateMonitor.reset();

    initialized = true;
}

bool UdpCom::read() {
    if (!initialized) {
        return false;
    }
    
    char buffer[1024];
    sockaddr_in senderAddr;
    int len = sizeof(senderAddr);

    int bytesRecieved = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&senderAddr, &len);
    
    if (bytesRecieved == SOCKET_ERROR) {
        int error = WSAGetLastError();
        return false;
    }
    
    if (bytesRecieved > 0) {
        bool successful = true;
        for (int i = 0; i < bytesRecieved; i++) {
            if (!SerialTask::packetReceiver.receiveByte(buffer[i])) {
                successful = false;
            }
        }

        return successful;
    }

    return false;
}

bool UdpCom::write(uint8_t* msg, size_t size) {
    if (!initialized) {
        return false;
    }

    int bytesSent = sendto(sock, reinterpret_cast<const char*>(msg), size, 0, (struct sockaddr*) &destAddr, sizeof(destAddr));

    if (bytesSent == SOCKET_ERROR) {
        return false;
    }

    return (static_cast<size_t>(bytesSent) == size);
}

bool UdpCom::comOpened() {
    return (sock != INVALID_SOCKET && initialized);
}

void UdpCom::shutdown() {
    if (sock != INVALID_SOCKET) {
        closesocket(sock);
        sock = INVALID_SOCKET;
    }

    WSACleanup();
    initialized = false;
}

bool UdpCom::getPacket(uint8_t* recv) {
    return SerialTask::packetReceiver.getPacket(recv);
}

uint8_t* UdpCom::getBuffer() {
    return SerialTask::packetReceiver.getBuffer();
}