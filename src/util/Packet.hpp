#pragma once

#include <vector>
#include <string>
#include <cstdint>

class Client;
class Server;

class Packet {
private:
    std::vector<char> buffer;
    size_t readPos = 0;

public:
    uint16_t senderID = 0;

    void appendData(const void *data, size_t length);

    void releaseData(void *dataOut, size_t length);

    const void *getData();

    size_t size();

    Packet& operator <<(int8_t data);

    Packet& operator <<(uint8_t data);

    Packet& operator <<(int16_t data);

    Packet& operator <<(uint16_t data);

    Packet& operator <<(int32_t data);

    Packet& operator <<(uint32_t data);

    Packet& operator <<(int64_t data);

    Packet& operator <<(uint64_t data);

    Packet& operator <<(float data);

    Packet& operator <<(double data);

    Packet& operator <<(const std::string& data);



    Packet& operator >>(int8_t &data);

    Packet& operator >>(uint8_t &data);

    Packet& operator >>(int16_t &data);

    Packet& operator >>(uint16_t &data);

    Packet& operator >>(int32_t &data);

    Packet& operator >>(uint32_t &data);

    Packet& operator >>(int64_t &data);

    Packet& operator >>(uint64_t &data);

    Packet& operator >>(float &data);

    Packet& operator >>(double &data);

    Packet& operator >>(std::string &data);
};