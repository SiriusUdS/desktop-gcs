#pragma once

#include "Logging.h"

template <size_t BUFSIZE>
class CircularBuffer {
public:
    bool read(uint8_t* recv, size_t size);
    bool dump(size_t size);
    bool writeByte(uint8_t c);
    void clear();
    std::optional<uint8_t> peekBack(size_t offset = 0) const;
    bool isFull() const;
    size_t readAvailable() const;
    size_t writeAvailable() const;
    uint8_t* getBuffer();

private:
    size_t nextIndex(size_t idx, size_t increment = 1) const;
    size_t prevIndex(size_t idx, size_t decrement = 1) const;

    size_t readIdx{};
    size_t writeIdx{};
    uint8_t buf[BUFSIZE]{};
    bool bufFull{};
};

template <size_t BUFSIZE>
inline bool CircularBuffer<BUFSIZE>::read(uint8_t* recv, size_t size) {
    const size_t READ_AVAILABLE = readAvailable();
    if (READ_AVAILABLE < size) {
        GCS_APP_LOG_WARN("CircularBuffer: Tried to read {} bytes, but only {} bytes are available.", size, READ_AVAILABLE);
        return false;
    }

    if (size == 0) {
        return false;
    }

    for (size_t idx = 0; idx < size; idx++) {
        recv[idx] = buf[readIdx];
        readIdx = nextIndex(readIdx);
    }

    bufFull = false;
    return true;
}

template <size_t BUFSIZE>
inline bool CircularBuffer<BUFSIZE>::dump(size_t size) {
    const size_t READ_AVAILABLE = readAvailable();
    if (READ_AVAILABLE < size) {
        GCS_APP_LOG_WARN("CircularBuffer: Tried to dump {} bytes, but only {} bytes are available.", size, READ_AVAILABLE);
        return false;
    }

    readIdx = nextIndex(readIdx, size);
    bufFull = false;
    return true;
}

template <size_t BUFSIZE>
inline bool CircularBuffer<BUFSIZE>::writeByte(uint8_t c) {
    if (bufFull) {
        return false;
    }

    buf[writeIdx] = c;
    writeIdx = nextIndex(writeIdx);
    bufFull = (writeIdx == readIdx);

    return true;
}

template <size_t BUFSIZE>
inline void CircularBuffer<BUFSIZE>::clear() {
    readIdx = 0;
    writeIdx = 0;
    bufFull = false;
}

template <size_t BUFSIZE>
inline std::optional<uint8_t> CircularBuffer<BUFSIZE>::peekBack(size_t offset) const {
    if (offset >= readAvailable()) {
        return {};
    }
    return buf[prevIndex(writeIdx, offset + 1)];
}

template <size_t BUFSIZE>
inline bool CircularBuffer<BUFSIZE>::isFull() const {
    return bufFull;
}

template <size_t BUFSIZE>
inline size_t CircularBuffer<BUFSIZE>::readAvailable() const {
    if (bufFull) {
        return BUFSIZE;
    } else if (readIdx < writeIdx) {
        return writeIdx - readIdx;
    } else if (readIdx > writeIdx) {
        return BUFSIZE - (readIdx - writeIdx);
    } else {
        return 0;
    }
}

template <size_t BUFSIZE>
inline size_t CircularBuffer<BUFSIZE>::writeAvailable() const {
    if (bufFull) {
        return 0;
    } else if (writeIdx < readIdx) {
        return readIdx - writeIdx;
    } else if (readIdx < writeIdx) {
        return BUFSIZE - (writeIdx - readIdx);
    } else {
        return BUFSIZE;
    }
}

template <size_t BUFSIZE>
inline uint8_t* CircularBuffer<BUFSIZE>::getBuffer() {
    return buf;
}

template <size_t BUFSIZE>
inline size_t CircularBuffer<BUFSIZE>::nextIndex(size_t idx, size_t increment) const {
    return (idx + increment) % BUFSIZE;
}

template <size_t BUFSIZE>
inline size_t CircularBuffer<BUFSIZE>::prevIndex(size_t idx, size_t decrement) const {
    decrement %= BUFSIZE;
    if (decrement > idx) {
        idx += BUFSIZE;
    }
    return idx - decrement;
}
