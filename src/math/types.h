#pragma once
#include "E:\Code\lib\eigen-3.3.4\Eigen/Core"
//#include "crazyflie/crtp_packet.h"
#include <vector>
#include <memory>
#include <stdint.h>
#include "text_logger.h"

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
    void Print(std::ostream & stream)
    {
         stream << "id = " << static_cast<int>(id )<< " name = " << name << " type = " << static_cast<int>(type )<< " value = "<< value << "\n";
    }
};
struct Point2f
{
    float x;
    float y;
    Point2f():
        x(0.0f),y(0.0f) {}
    Point2f(float xx, float yy):
        x(xx),y(yy){}
};
struct Point3f
{
   float x;
   float y;
   float z;
   Point3f():
       x(0.0f),y(0.0f), z(0.0f) {}
   Point3f(float xx, float yy, float zz):
       x(xx),y(yy), z(zz) {}
};


// (0, 0) is in the middle of the camera view
// x is to the left
// y is to the top
struct MidPoint
{
    Point2f pt; // TODO Move to app/types.h
    float size;
    MidPoint():
        pt(0.0f,0.0f), size(0.0f) {}
    MidPoint(float xx, float yy, float zz):
        pt(xx,yy), size(zz) {}
};


template<typename T>
T GetLargest(std::vector<T> const & elements)
{
    T largestElement;
    if(elements.size() == 1)
    {
        largestElement =  elements.at(0);
    }
    else if(elements.size() > 1)
    {
        for(auto const & element : elements)
        {
            if(element.size > largestElement.size)
            {
                largestElement = element;
            }
        }
    }
    return largestElement;
}

void PrintData(Data const & data);

