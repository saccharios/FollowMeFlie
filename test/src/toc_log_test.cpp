#include "toc_log_test.h"
// toc_test is friend of toc_log

void TOC_Log_Test::Run_TestOk()
{
    RadioDongle radioDongle;
    TocLog toc(radioDongle);


    TOCElement tocElement1;
    tocElement1.id = 7;
    tocElement1.type = ElementType::FLOAT;
    tocElement1.value = 0;
    toc._elements.push_back(tocElement1);
    TOCElement tocElement2;
    tocElement2.id = 42;
    tocElement2.value = 0;
    tocElement2.type = ElementType::FLOAT;
    toc._elements.push_back(tocElement2);
    TOCElement tocElement3;
    tocElement3.id = 0;
    tocElement3.value = 0;
    tocElement3.type = ElementType::UINT8;
    toc._elements.push_back(tocElement3);
    TOCElement tocElement4;
    tocElement4.id = 5;
    tocElement4.value = 0;
    tocElement4.type = ElementType::UINT8;
    toc._elements.push_back(tocElement4);

    TocLog::LoggingBlock block1;
    block1.id = blockID1;
    block1.elements.push_back(&toc._elements.at(0));
    block1.elements.push_back(&toc._elements.at(1));
    toc._loggingBlocks.push_back(block1);
    TocLog::LoggingBlock block2;
    block2.id = blockID2;
    block2.elements.push_back(&toc._elements.at(2));
    block2.elements.push_back(&toc._elements.at(3));
    toc._loggingBlocks.push_back(block2);


    // All is ok
    toc.ProcessLogPackets(_packets);
    EXPECT_FLOAT_EQ(toc._elements.at(0).value, num1);
    EXPECT_FLOAT_EQ(toc._elements.at(1).value, num2);
    EXPECT_FLOAT_EQ(toc._elements.at(2).value, int1);
    EXPECT_FLOAT_EQ(toc._elements.at(3).value, int2);
}

void TOC_Log_Test::Run_GetFirstFreeID()
{
    RadioDongle radioDongle;
    TocLog toc(radioDongle);

    // Empty logginBlocks Vector
    int id = toc.GetFirstFreeID();
    EXPECT_EQ(0,id);

    // Setup TOC
    TocLog::LoggingBlock block1;
    block1.id = 0;
    TocLog::LoggingBlock block2;
    block2.id = 1;
    TocLog::LoggingBlock block3;
    block3.id = 3;
    toc._loggingBlocks.emplace_back(block1);
    toc._loggingBlocks.emplace_back(block2);
    toc._loggingBlocks.emplace_back(block3);

    id = toc.GetFirstFreeID();
    EXPECT_EQ(2, id);
    TocLog::LoggingBlock block4;
    block4.id = 2;
    toc._loggingBlocks.emplace_back(block4);
    id = toc.GetFirstFreeID();
    EXPECT_EQ( 4, id);
}




