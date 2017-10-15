#pragma once
#include <array>

#include "E:\Code\lib\eigen-3.3.4\Eigen/Core"

using Acceleration = std::array<float,3>;
using Velocity = std::array<float,3>;

std::array<float,3> operator*(std::array<float,3> const & rhs, float const & scalar);

std::array<float,3> operator*( float const & scalar, std::array<float,3> const & rhs);

void operator+=(std::array<float,3> & lhs, std::array<float,3> const & rhs );

std::array<float,3> operator+(std::array<float,3> const & lhs, std::array<float,3> const & rhs );


using Matrix12 = Eigen::Matrix<float,12,12>;


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
    FLOAT = 7
};


struct TOCElement
{
    uint8_t id;
    std::string group;
    std::string name_only;
    std::string name; // TODO SF Is combined name : group +"."+name_only
    ElementType type;
    float value;
};
