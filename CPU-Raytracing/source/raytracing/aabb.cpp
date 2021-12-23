#include "aabb.h"

namespace CRT
{
	AABB AABB::NegativeBox()
	{
		float3 min = float3::Infinity();
		float3 max = float3::NegativeInfinity();
		return { min, max };
	}

	AABB AABB::Extend(AABB other)
	{
		return { Min.ComponentMin(other.Min), Max.ComponentMax(other.Max) };
	}

	AABB AABB::Extend(float3 point)
	{
		return { Min.ComponentMin(point), Max.ComponentMax(point) };
	}
}