#include "util/Packet.hpp"

#include "client/Client.hpp"
#include "server/Server.hpp"

#include <iostream>


void Packet::appendData(const void *data, size_t length) {
    if (data && (length > 0))
    {
        size_t start = buffer.size();
        buffer.resize(start + length);
        memcpy(&buffer[start], data, length);
    }
}

void Packet::releaseData(void *dataOut, size_t length) {
    if (length <= (size() - readPos)) {
        memcpy(dataOut, buffer.data() + readPos, length);

        readPos += length;
    }
}

const void* Packet::getData() {
    return &buffer[0] ;
}

size_t Packet::size() {
    return buffer.size();
}

Packet& Packet::operator<<(int8_t data) {
    appendData(&data, sizeof(data));
    return *this;
}
Packet& Packet::operator<<(uint8_t data) {
    appendData(&data, sizeof(data));
    return *this;
}
Packet& Packet::operator<<(int16_t data) {
    appendData(&data, sizeof(data));
    return *this;
}
Packet& Packet::operator<<(uint16_t data) {
    appendData(&data, sizeof(data));
    return *this;
}
Packet& Packet::operator<<(int32_t data) {
    appendData(&data, sizeof(data));
    return *this;
}
Packet& Packet::operator<<(uint32_t data) {
    appendData(&data, sizeof(data));
    return *this;
}
Packet& Packet::operator<<(int64_t data) {
    appendData(&data, sizeof(data));
    return *this;
}
Packet& Packet::operator<<(uint64_t data) {
    appendData(&data, sizeof(data));
    return *this;
}
Packet& Packet::operator<<(float data) {
    appendData(&data, sizeof(data));
    return *this;
}
Packet& Packet::operator<<(double data) {
    appendData(&data, sizeof(data));
    return *this;
}
Packet& Packet::operator<<(const std::string &data) {
    uint32_t length = data.size();

    *this << length;
    appendData(data.data(), length);

    return *this;
}


Packet& Packet::operator>>(int8_t &data) {
    if (size() >= readPos + sizeof(data)) {
        data = *reinterpret_cast<const int8_t*>(&buffer[readPos]);
        readPos += sizeof(data);
    }
    return *this;
}
Packet& Packet::operator>>(uint8_t &data) {
    if (size() >= readPos + sizeof(data)) {
        data = *reinterpret_cast<const uint8_t*>(&buffer[readPos]);
        readPos += sizeof(data);
    }
    return *this;
}
Packet& Packet::operator>>(int16_t &data) {
    if (size() >= readPos + sizeof(data)) {
        data = *reinterpret_cast<const int16_t*>(&buffer[readPos]);
        readPos += sizeof(data);
    }
    return *this;
}
Packet& Packet::operator>>(uint16_t &data) {
    if (size() >= readPos + sizeof(data)) {
        data = *reinterpret_cast<const uint16_t*>(&buffer[readPos]);
        readPos += sizeof(data);
    }
    return *this;
}
Packet& Packet::operator>>(int32_t &data) {
    if (size() >= readPos + sizeof(data)) {
        data = *reinterpret_cast<const int32_t*>(&buffer[readPos]);
        readPos += sizeof(data);
    }
    return *this;
}
Packet& Packet::operator>>(uint32_t &data) {
    if (size() >= readPos + sizeof(data)) {
        data = *reinterpret_cast<const uint32_t*>(&buffer[readPos]);
        readPos += sizeof(data);
    }
    return *this;
}
Packet& Packet::operator>>(int64_t &data) {
    if (size() >= readPos + sizeof(data)) {
        data = *reinterpret_cast<const int64_t*>(&buffer[readPos]);
        readPos += sizeof(data);
    }
    return *this;
}
Packet& Packet::operator>>(uint64_t &data) {
    if (size() >= readPos + sizeof(data)) {
        data = *reinterpret_cast<const uint64_t*>(&buffer[readPos]);
        readPos += sizeof(data);
    }
    return *this;
}
Packet& Packet::operator>>(float &data) {
    if (size() >= readPos + sizeof(data)) {
        data = *reinterpret_cast<const float*>(&buffer[readPos]);
        readPos += sizeof(data);
    }
    return *this;
}
Packet& Packet::operator>>(double &data) {
    if (size() >= readPos + sizeof(data)) {
        data = *reinterpret_cast<const double*>(&buffer[readPos]);
        readPos += sizeof(data);
    }
    return *this;
}
Packet& Packet::operator>>(std::string &data) {
    uint32_t length = 0;
    *this >> length;

    data.clear();
    if (size() >= readPos + length && length > 0) {
        // Then extract characters
        data.assign(&buffer[readPos], length);

        // Update reading position
        readPos += length;
    }
    return *this;
}