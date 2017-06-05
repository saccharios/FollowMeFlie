#pragma once
#include "gtest/gtest.h"
#include <iostream>
#include <chrono>
#include "communication/CRTPPacket.h"


class CRTPPacketTest : public ::testing::Test
{
public:
private:
};

TEST_F(CRTPPacketTest, Construction)
{

    std::vector<uint8_t> data;
    data.push_back(0);
    data.push_back(1);
    data.push_back(2);
    data.push_back(51);
    Port port = Port::Log;
    Channel channel = Channel::Data;

    CRTPPacket packet(port, channel, std::move(data));
    EXPECT_EQ(packet.GetChannel(), channel);
    EXPECT_EQ(packet.GetPort(), port);
    //    EXPECT_DEATH(CRTPPacket(7, 0, data), "");
    //    EXPECT_DEATH(CRTPPacket(0, 7, data), "");
    //    EXPECT_DEATH(CRTPPacket(0, -2, data), "");
    int header = (static_cast<int>(port) << 4) | 0b00001100 | (static_cast<int>(channel) & 0x03); // 0x03
    EXPECT_EQ(static_cast<int>(packet.SendableData()[0]), header);

    for(unsigned int i = 0; i < data.size(); ++i)
    {
        EXPECT_EQ(static_cast<int>(packet.GetData()[i]), static_cast<int>(data[i]));
        EXPECT_EQ(static_cast<int>(packet.SendableData()[i+1]), static_cast<int>(data[i]));
    }
}

TEST_F(CRTPPacketTest, Convert_Float_To_uint8_tVect)
{
    float num = 0.7;
    uint8_t buffer[sizeof(float)];
    memcpy(&buffer[0], &num, sizeof(float));
    auto myVect = ConvertTouint8_tVect(num);
    EXPECT_EQ(myVect.size(), sizeof(float));
    for(unsigned int i = 0; i <myVect.size(); ++i)
    {
        EXPECT_EQ(myVect[i], buffer[i]);
    }
}
TEST_F(CRTPPacketTest, Convert_Double_To_uint8_tVect)
{
    double num = 0.7;
    uint8_t buffer[sizeof(double)];
    memcpy(&buffer[0], &num, sizeof(double));
    auto myVect = ConvertTouint8_tVect(num);
    EXPECT_EQ(myVect.size(), sizeof(double));
    for(unsigned int i = 0; i <myVect.size(); ++i)
    {
        EXPECT_EQ(myVect[i], buffer[i]);
    }
}
TEST_F(CRTPPacketTest, Convert_Int_To_uint8_tVect)
{
    int num = 1897;
    uint8_t buffer[sizeof(int)];
    memcpy(&buffer[0], &num, sizeof(int));
    auto myVect = ConvertTouint8_tVect(num);
    EXPECT_EQ(myVect.size(), sizeof(int));
    for(unsigned int i = 0; i <myVect.size(); ++i)
    {
        EXPECT_EQ(myVect[i], buffer[i]);
    }
}
TEST_F(CRTPPacketTest, Convert_Short_To_uint8_tVect)
{
    short num = 79;
    uint8_t buffer[sizeof(short)];
    memcpy(&buffer[0], &num, sizeof(short));
    auto myVect = ConvertTouint8_tVect(num);
    EXPECT_EQ(myVect.size(), sizeof(short));
    for(unsigned int i = 0; i <myVect.size(); ++i)
    {
        EXPECT_EQ(myVect[i], buffer[i]);
    }
}
TEST_F(CRTPPacketTest, Convert_uint8_t_To_uint8_tVect)
{
    uint8_t num = 7;
    uint8_t buffer[sizeof(uint8_t)];
    memcpy(&buffer[0], &num, sizeof(uint8_t));
    auto myVect = ConvertTouint8_tVect(num);
    EXPECT_EQ(myVect.size(), sizeof(uint8_t));
    for(unsigned int i = 0; i <myVect.size(); ++i)
    {
        EXPECT_EQ(myVect[i], buffer[i]);
    }
}
//TEST_F(CRTPPacketTest, TimerCopyCtorVsMoveCtor)
//{
//    std::vector<uint8_t> data;
//    data.push_back(0);
//    data.push_back(1);
//    data.push_back(2);
//    data.push_back(51);
//    int port = 5;
//    int channel = 2;

//    CRTPPacket packet_1(port, channel, data);
//    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
//    CRTPPacket packet_2 = packet_1; // copy ctor
//    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
//    std::cout << "Copy Ctor"
//              << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
//              << "ns.\n";
//    start = std::chrono::steady_clock::now();
//    CRTPPacket packet_3 = std::move(packet_1);
//    end = std::chrono::steady_clock::now();
//    std::cout << "Move Ctor"
//              << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
//              << "ns.\n";
//    std::cout << "packet 3 data length" << packet_3.DataLength() << std::endl;
//}
TEST_F(CRTPPacketTest, TimerCtorDataVectMove)
{
    std::vector<uint8_t> data;
    data.push_back(0);
    data.push_back(1);
    data.push_back(2);
    data.push_back(51);
    Port port = Port::Log;
    Channel channel = Channel::Data;

    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    CRTPPacket packet_2(port, channel, std::move(data));
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Move Data Ctor"
              << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
              << "ns.\n";
}

