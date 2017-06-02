#pragma once
#include "gtest/gtest.h"
#include <iostream>
#include <algorithm>
class STLUtils : public ::testing::Test
{
public:

    template<typename T>
    static bool  VectorContains(std::vector<T> const & vect, T element)
    {
        std::cout << "using const lvalue ref\n";
        return std::find(vect.begin(), vect.end(), element) != vect.end();
    }

    template<typename T>
    static bool  VectorContains(std::vector<T> && vect, T element)
    {
        std::cout << "using rvalue ref\n";
        return std::find(vect.begin(), vect.end(), element) != vect.end();
    }
    template<typename T>
    typename std::vector<T> ::iterator  IteratorToElement(std::vector<T> && vect, T element, bool & isValid)
    {
        auto it = std::find(vect.begin(), vect.end(), element);
        isValid = it != vect.end();
        return  it;
    }


private:
};

TEST_F(STLUtils, VectorContains)
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
TEST_F(STLUtils, VectorIterator)
{

    int element =9;
    std::vector<int> myVect{4,798,7,4,31,6};

    bool isValid = false;
    auto it = STLUtils::IteratorToElement({1,38,64,7}, element, isValid);
    if(isValid)
    {
        std::cout << "Element " << element << " in Vector: " << "isContained" << *it<< std::endl;
    }
    else
    {
        // Element is not found
        std::cout << "Element " << element << " is not in the vector\n";

    }

}
