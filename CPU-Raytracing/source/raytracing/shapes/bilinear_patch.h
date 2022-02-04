#pragma once

#include <./core/math/float3.h>
#include <./raytracing/ray.h>

namespace CRT
{
	class Manifest;

	struct BilinearPatch
	{
		BilinearPatch(float3 _lowerLeft, float3 _lowerRight, float3 _upperLeft, float3 _upperRight);

		bool Intersect(Ray _ray, Manifest& man) const;

		float3 LowerLeft;
		float3 LowerRight;
		float3 UpperLeft;
		float3 UpperRight;
		float3 Normal;
	};
}
