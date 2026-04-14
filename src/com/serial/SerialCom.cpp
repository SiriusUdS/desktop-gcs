#include "SerialCom.h"

#include "BoardComStateMonitor.h"
#include "ComPortSelector.h"
#include "ComTask.h"
#include "PacketRateMonitor.h"
#include "PacketReceiver.h"


/**
 * @brief Initializes communication on the first COM port found.
 */
void SerialCom::start() {
    com.Close();
    ComTask::packetRateMonitor.reset();
    ComTask::motorBoardComStateMonitor.reset();
    ComTask::fillingStationBoardComStateMonitor.reset();
    comPortSelector.next();

    com.SetPortName("\\\\.\\" + comPortSelector.current());
    com.SetBaudRate(CBR_19200);
    com.Open();
}

/**
 * @brief Reads a single byte from the COM port into an internal buffer.
 * @returns True if bytes were read, otherwise false
 * necessarily indicate an error.
 */
bool SerialCom::read() {
    static constexpr size_t BYTES_TO_READ_PER_SECOND = 19'200;
    static constexpr size_t MAX_BYTES_TO_READ_PER_TASK_LOOP = 19'200 / SerialConfig::SERIAL_TASK_LOOPS_PER_SECOND;

    double elapsedSeconds = timerSerialRead.getElapsedTimeInSeconds();
    timerSerialRead.reset();

    size_t bytesToRead = std::min<size_t>(MAX_BYTES_TO_READ_PER_TASK_LOOP, (size_t) (BYTES_TO_READ_PER_SECOND * elapsedSeconds));
    while (bytesToRead--) {
        bool successful;
        char c = com.ReadChar(successful);
        if (successful) {
            successful = ComTask::packetReceiver.receiveByte(c);
        }

        if (!successful) {
            return false;
        }
    }

    return true;
}

/**
 * @brief Send data through the opened COM port.
 * @param msg The char buffer that contains the data to send.
 * @param size The size of the data to send.
 * @returns True if the data was successfully sent, otherwise false.
 */
bool SerialCom::write(uint8_t* msg, size_t size) {
    static constexpr size_t WRITE_TIMEOUT_MS = 100;
    bool successful = com.WriteArr(msg, (long) size, WRITE_TIMEOUT_MS);
    return successful;
}

/**
 * @brief Checks if a COM port is opened.
 * @returns True if a COM port is opened, otherwise false.
 */
bool SerialCom::comOpened() {
    return com.IsOpened();
}

/**
 * @brief Fetches a packet from the internal buffer if one is available.
 * @param recv The char buffer to receive the bytes into.
 * @returns True if a packet was successfully received, otherwise false.
 */
bool SerialCom::getPacket(uint8_t* recv) {
    return ComTask::packetReceiver.getPacket(recv);
}

/**
 * @brief Gets a pointer to the internal buffer used for receiving data.
 * @returns Pointer to the internal buffer.
 */
uint8_t* SerialCom::getBuffer() {
    return ComTask::packetReceiver.getBuffer();
}

/**
 * @brief Shuts down communication with the currently opened serial COM port.
 */
void SerialCom::shutdown() {
    com.Close();
}

std::string SerialCom::getProtocolName() {
    return protocolNameFromEnum(comType);
}

std::optional<std::string> SerialCom::getConnectionDetails() {
    if (comPortSelector.available()) {
        return comPortSelector.current();
    }
    return std::nullopt;
}

ComType SerialCom::getComType() const {
    return comType;
}
