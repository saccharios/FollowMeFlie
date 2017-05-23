#ifndef SIMPLETEST_H
#define SIMPLETEST_H
#include "gtest/gtest.h"
#include "communication/CRTPPacket.h"
#include <iostream>

class CRTPPacketTest : public ::testing::Test
{
public:
private:
};

TEST_F(CRTPPacketTest, ConstructionTest)
{

    std::vector<char> data;
    data.push_back(0);
    data.push_back(1);
    data.push_back(2);
    data.push_back(51);
    int port = 5;
    int channel = 2;

    CRTPPacket packet(port, channel, data);
    EXPECT_EQ(packet.DataLength(), data.size());
    EXPECT_EQ(packet.GetChannel(), channel);
    EXPECT_EQ(packet.GetPort(), port);
//    EXPECT_DEATH(CRTPPacket(7, 0, data), "");
//    EXPECT_DEATH(CRTPPacket(0, 7, data), "");
    int header = (port << 4) | 0b00001100 | (channel & 0x03); // 0x03
    EXPECT_EQ(static_cast<int>(packet.SendableData()[0]), header);

    for(unsigned int i = 0; i < data.size(); ++i)
    {
        EXPECT_EQ(static_cast<int>(packet.Data()[i]), static_cast<int>(data[i]));
        EXPECT_EQ(static_cast<int>(packet.SendableData()[i+1]), static_cast<int>(data[i]));
    }



}

#endif // SIMPLETEST_H
