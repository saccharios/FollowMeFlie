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


