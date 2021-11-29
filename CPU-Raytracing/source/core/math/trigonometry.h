#pragma once

namespace CRT
{
	template<typename T>
	constexpr T Pi()
	{
		return (T)3.14159265358979323846;
	}

	template<typename T>
	constexpr T ToRadians(T degrees)
	{
		return (degrees / 180.0f) * Pi<T>();
	}

	template<typename T>
	constexpr T ToDegrees(T radians)
	{
		return (radians * 180.0f) / Pi<T>();
	}
}
