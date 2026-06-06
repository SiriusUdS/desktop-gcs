#include "UdpTestCommands.h"

#include "ComTask.h"
#include "CRC.h"
#include "ICom.h"
#include "UDPTelemetryPacket.h"

#include <ctime>

void UdpTestCommands::testValve(uint16_t valueToSend) {
    ServoPacket servoPacket;
    
    servoPacket.frame.status = SERVO_MANUAL; 
    servoPacket.frame.reserved = 0; 
    servoPacket.frame.value = valueToSend;
    
    servoPacket.frame.header.frame.deviceId = 0x01; // Example device ID
    servoPacket.frame.header.frame.payloadId = SET_SERVO; // Command to set servo
    servoPacket.frame.header.frame.payloadLength = sizeof(servoPacket.data) - sizeof(servoPacket.frame.crc);
    servoPacket.frame.header.frame.deviceState = 0; // Example device state
    servoPacket.frame.header.frame.reserved = 0;
    servoPacket.frame.header.frame.deviceTsMs = 0; //Example
    
    servoPacket.frame.crc = CRC::computeCrcUDP(servoPacket.data, sizeof(servoPacket.data) - sizeof(servoPacket.frame.crc));
    ComTask::com->write(servoPacket.data);
}
