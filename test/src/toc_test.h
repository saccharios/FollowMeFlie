#pragma once

#include "gtest/gtest.h"
#include <iostream>
#include "communication/TOC.h"
#include "communication/CRTPPacket.h"
#include "communication/CrazyRadio.h"
#include <memory>

class TOC_Test : public testing::Test
{
    // TOC_Test if friend of TOC
private:
    using sptrPacket = std::shared_ptr<CRTPPacket>;
     std::vector<sptrPacket> _packets;
     Port _port = Port::Log;
     Channel _channel = Channel::Data;
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
         std::vector<uint8_t> data = {0,blockID1,0,0,0};
         data.insert(std::end(data), std::begin(data1), std::end(data1));
         data.insert(std::end(data), std::begin(data2), std::end(data2));
        std::cout << "float packet data size = " << data.size() << std::endl;
         auto packet1 = std::make_shared<CRTPPacket>(_port, _channel, std::move(data));
         _packets.emplace_back(std::move(packet1));
         std::vector<uint8_t> load =  {0,blockID2,0,0,0,int1, int2};
         auto packet2 = std::make_shared<CRTPPacket>(_port, _channel,std::move(load));
         _packets.emplace_back(std::move(packet2));
     }



    void
    run_test()
    {
        CrazyRadio crazyRadio;
        TOC toc(crazyRadio, _port);
        // Setup TOC
        int element_id1 = 7;
        int element_id2 = 42;
        int element_id3 = 0;
        int element_id4 = 5;
        LoggingBlock block1;
        block1.id = blockID1;
        block1.elementIDs.push_back(element_id3);
        block1.elementIDs.push_back(element_id4);
        toc._loggingBlocks.push_back(block1);
        LoggingBlock block2;
        block2.id = blockID2;
        block2.elementIDs.push_back(element_id1);
        block2.elementIDs.push_back(element_id2);
        toc._loggingBlocks.push_back(block2);


        TOCElement tocElement;
        tocElement.id = element_id1;
        tocElement.type = ElementType::UINT8;
        tocElement.value = 0;
        toc._TOCElements.push_back(tocElement);
        tocElement.id = element_id2;
        tocElement.value = 0;
        tocElement.type = ElementType::UINT8;
        toc._TOCElements.push_back(tocElement);
        tocElement.id = element_id3;
        tocElement.value = 0;
        tocElement.type = ElementType::FLOAT;
        toc._TOCElements.push_back(tocElement);
        tocElement.id = element_id4;
        tocElement.value = 0;
        tocElement.type = ElementType::FLOAT;
        toc._TOCElements.push_back(tocElement);

        toc.ProcessLogPackets(_packets);
        EXPECT_FLOAT_EQ(toc._TOCElements.at(0).value, int1);
        EXPECT_FLOAT_EQ(toc._TOCElements.at(1).value, int2);
        EXPECT_FLOAT_EQ(toc._TOCElements.at(2).value, num1);
        EXPECT_FLOAT_EQ(toc._TOCElements.at(3).value, num2);


    }
};



TEST_F(TOC_Test, ProcessLogPackets)
{
    run_test();
}
