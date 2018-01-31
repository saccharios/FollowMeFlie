#pragma once
#include "E:\Code\lib\eigen-3.3.4\Eigen/Core"
//#include "crazyflie/crtp_packet.h"
#include <vector>
#include <iostream>
#include <memory>
#include <stdint.h>

using Acceleration = Eigen::Vector3f;
using Velocity = Eigen::Vector3f;
using Data = std::vector<uint8_t>;

union IntFloat
{
    float float_value;
    int int_value;
};

enum class ElementType : uint8_t
{
    UINT8  = 1,
    UINT16  = 2,
    UINT32  = 3,
    INT8  = 4,
    INT16  = 5,
    INT32  = 6,
    FLOAT =7,
    UINT64  = 11,
    INT64  = 12,
    DOUBLE = 13,
    FP16 = 14
};

struct TOCElement
{
    uint8_t id;
    std::string group;
    std::string name_only;
    std::string name; // TODO SF Is combined name : group +"."+name_only
    ElementType type;
    float value;
    bool isLogged;
    void Print()
    {
        std::cout << "id = " << static_cast<int>(id )<< " name = " << name << " type = " << static_cast<int>(type )<< " value = "<< value << std::endl;
    }
};
struct Distance
{
   float x;
   float y;
   float z;
};
