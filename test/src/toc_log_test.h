#pragma once

#include "gtest/gtest.h"
#include <iostream>
#include "communication/toc_log.h"
#include "communication/toc_parameter.h"
#include "communication/crtp_packet.h"
#include "communication/crazy_radio.h"
#include <memory>
#include "math/types.h"

class TOC_Log_Test : public testing::Test
{
    // TOC_Log_Test if friend of TOC Log
private:
    using sptrPacket = std::shared_ptr<CRTPPacket>;
    std::vector<sptrPacket> _packets;
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
         auto packet1 = std::make_shared<CRTPPacket>(_port, _channel, std::move(data));
         _packets.emplace_back(std::move(packet1));
         Data load =  {blockID2,0,0,0,int1, int2};
         auto packet2 = std::make_shared<CRTPPacket>(_port, _channel,std::move(load));
         _packets.emplace_back(std::move(packet2));
     }



    void Run_TestOk();
    void Run_GetFirstFreeID();

};



TEST_F(TOC_Log_Test, ProcessLogPacketsOK)
{
    Run_TestOk();
}
TEST_F(TOC_Log_Test, GetFirstFreeID)
{
    Run_GetFirstFreeID();
}
