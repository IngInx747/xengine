#pragma once
#ifndef XE_MATH_CONSTANT_H
#define XE_MATH_CONSTANT_H

namespace xengine
{
	constexpr float kEps = 1e-3f;
	constexpr float kInf = 1e20f;

	constexpr float kMax = kInf;
	constexpr float kMin = -kInf;

	constexpr float kPi6 = 0.5235987755982988f;
	constexpr float kPi4 = 0.7853981633974483f;
	constexpr float kPi3 = 1.0471975511965976f;
	constexpr float kPi2 = 1.5707963267948966f;
	constexpr float kPi  = 3.141592653589793f;
	constexpr float k2Pi = 6.283185307179586f;
}

#endif // !XE_MATH_CONSTANT_H
