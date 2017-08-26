#pragma once


using Acceleration = std::array<float,3>;
using Velocity = std::array<float,3>;

std::array<float,3> operator*(std::array<float,3> const & rhs, float const & scalar)
{
    std::array<float,3> lhs;
    lhs[0] = rhs[0] * scalar;
    lhs[1] = rhs[1] * scalar;
    lhs[2] = rhs[2] * scalar;
    return lhs;
}
std::array<float,3> operator*( float const & scalar, std::array<float,3> const & rhs)
{
    return rhs * scalar;
}

void operator+=(std::array<float,3> & lhs, std::array<float,3> const & rhs )
{
    lhs[0] = lhs[0] + rhs[0];
    lhs[1] = lhs[1] + rhs[1];
    lhs[2] = lhs[2] + rhs[2];
}
std::array<float,3> operator+(std::array<float,3> const & lhs, std::array<float,3> const & rhs )
{
    auto temp = lhs;
    temp += rhs;
    return temp;
}



