#pragma once
#include "gtest/gtest.h"
#include "math/logic.h"

class Logic_Test : public testing::Test
{

};


TEST_F(Logic_Test, PositiveEdgeDetectors)
{
    Positive_Edge_Detector pos_edge_det;


    EXPECT_FALSE(pos_edge_det.Update(false));
    EXPECT_TRUE(pos_edge_det.Update(true));
    EXPECT_FALSE(pos_edge_det.Update(true));
    EXPECT_FALSE(pos_edge_det.Update(true));
    EXPECT_FALSE(pos_edge_det.Update(false));
    EXPECT_FALSE(pos_edge_det.Update(false));
}

TEST_F(Logic_Test, NegativeEdgeDetectors)
{
    Negative_Edge_Detector neg_edge_det;

    EXPECT_FALSE(neg_edge_det.Update(true));
    EXPECT_TRUE(neg_edge_det.Update(false));
    EXPECT_FALSE(neg_edge_det.Update(false));
}
