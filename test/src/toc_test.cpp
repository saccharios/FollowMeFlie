#include "toc_test.h"


void TOC_Test::run_test_ok()
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

    // All is ok
    toc.ProcessLogPackets(_packets);
    EXPECT_FLOAT_EQ(toc._TOCElements.at(0).value, int1);
    EXPECT_FLOAT_EQ(toc._TOCElements.at(1).value, int2);
    EXPECT_FLOAT_EQ(toc._TOCElements.at(2).value, num1);
    EXPECT_FLOAT_EQ(toc._TOCElements.at(3).value, num2);
}

void TOC_Test::run_test_id_not_found()
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
    tocElement.id = 2; //<----------------Different ID
    tocElement.value = 0;
    tocElement.type = ElementType::FLOAT;
    toc._TOCElements.push_back(tocElement);

    EXPECT_DEATH(toc.ProcessLogPackets(_packets),"");

}
