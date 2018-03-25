#pragma once
#include "gtest/gtest.h"
#include <iostream>
#include "stl_utils.h"
#include "crazyflie/crtp_packet.h"

class STLUtils_Test : public testing::Test
{

};


TEST_F(STLUtils_Test, VectorContains)
{

    std::vector<int> myVect{4,798,7,4,31,6};

    int element = 7;
    bool isContained = STLUtils::VectorContains(myVect, element);
    std::cout << "Element " << element << " in Vector: " << isContained << std::endl;
    EXPECT_TRUE(isContained);
    isContained = STLUtils::VectorContains({1,38,64,7}, element);
    std::cout << "Element " << element << " in Vector: " << isContained << std::endl;
    EXPECT_TRUE(isContained);
}
TEST_F(STLUtils_Test, VectorIterator)
{

    int element = 9;

    bool isValid = false;
    auto it = STLUtils::IteratorToElement({1,38,64,7}, element, isValid);
    if(isValid)
    {
        std::cout << "Element " << element << " in Vector: " << "isContained" << it<< std::endl;
    }
    else
    {
        // Element is not found
        std::cout << "Element " << element << " is not in the vector\n";

    }
    EXPECT_FALSE(isValid);

}
TEST_F(STLUtils_Test, VectorSearchByID)
{
    struct Element{
        int id;
        std::string name;
        double data;
    };
    Element e1 = {0,"first",0.17};
    Element e2 = {1,"second",0.897};
    std::vector<Element> myVect = {e1,e2};

    bool isValid = false;
    int id = 0;
    auto & element1 = STLUtils::ElementForID(myVect, id, isValid);
    EXPECT_TRUE(isValid);
    EXPECT_EQ(element1.id, e1.id);
    id = 4;
    STLUtils::ElementForID(myVect, id, isValid);
    EXPECT_FALSE(isValid);

}
TEST_F(STLUtils_Test, VectorISearchByName)
{
    struct Element{
        int id;
        std::string name;
        double data;
    };
    Element e1 = {0,"first",0.17};
    Element e2 = {1,"second",0.897};
    std::vector<Element> myVect = {e1,e2};

    bool isValid = false;
    std::string name = "first";
    auto & element1 = STLUtils::ElementForName(myVect, name, isValid);
    EXPECT_TRUE(isValid);
    EXPECT_EQ(element1.id, e1.id);
    name = "abc";
    STLUtils::ElementForName(myVect, name, isValid);
    EXPECT_FALSE(isValid);
}




