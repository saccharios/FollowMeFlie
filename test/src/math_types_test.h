#pragma once
#include "gtest/gtest.h"
#include "math/types.h"
class Vector3D_Test : public testing::Test
{

};

TEST_F(Vector3D_Test, OperatorP)

{
    Velocity vel1 = {0.1,0.2,0.3};
    Velocity vel2= {1.1,1.2,1.3};
    auto vel3 = vel1 + vel2;
    for(int i = 0; i < 3; i++)
    {
        EXPECT_EQ(vel3[i], vel1[i] + vel2[i]);
    }

}
TEST_F(Vector3D_Test, OperatorPE)

{
    Velocity vel1 = {0.1,0.2,0.3};
    Velocity vel2= {1.1,1.2,1.3};

    auto vel3 = vel1;
    vel3 += vel2;
    for(int i = 0; i < 3; i++)
    {
        EXPECT_EQ(vel3[i], vel1[i] + vel2[i]);
    }
}

TEST_F(Vector3D_Test, OperatorMR)
{
    Velocity vel1 = {0.1,0.2,0.3};

    float a = 0.5;
    auto vel3 = a * vel1;
    for(int i = 0; i < 3; i++)
    {
        EXPECT_EQ(vel3[i], vel1[i] * a);
    }

}
TEST_F(Vector3D_Test, OperatorML)
{
    Velocity vel1 = {0.1,0.2,0.3};

    float a = 0.5;
    auto vel2 =  vel1*a;
    for(int i = 0; i < 3; i++)
    {
        EXPECT_EQ(vel2[i], vel1[i] * a);
    }

}
