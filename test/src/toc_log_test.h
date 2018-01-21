#pragma once

#include "gtest/gtest.h"
#include <iostream>
#include "crazyflie/toc_log.h"
#include "crazyflie/toc_parameter.h"
#include "crazyflie/crtp_packet.h"
#include "crazyflie/radio_dongle.h"
#include <memory>
#include "math/types.h"

class TOC_Log_Test : public testing::Test
{
    // TOC_Log_Test if friend of TOC Log
private:
    std::vector<CRTPPacket> _packets;
     int _port = 5; // Log Port
     int _channel = 2; // Data channel
     float num1 = 1.6756;
     float num2 = 73.6756;
     uint8_t int1 = 17;
     uint8_t int2 = 39;
     uint8_t blockID1 = 2;
     uint8_t blockID2 = 42;

public:
     void SetUp()
     {
         Test::SetUp();
         // Generate two packets
         auto data1 = ConvertTouint8_tVect(num1);
         auto data2 = ConvertTouint8_tVect(num2);
         Data data = {blockID1,0,0,0};
         data.insert(std::end(data), std::begin(data1), std::end(data1));
         data.insert(std::end(data), std::begin(data2), std::end(data2));
         CRTPPacket packet1(_port, _channel, std::move(data));
         _packets.emplace_back(packet1);
         Data load =  {blockID2,0,0,0,int1, int2};
         CRTPPacket packet2(_port, _channel, std::move(load));
         _packets.emplace_back(packet2);
     }



    void Run_TestOk();

};



TEST_F(TOC_Log_Test, ProcessLogPacketsOK)
{
    Run_TestOk();
}
