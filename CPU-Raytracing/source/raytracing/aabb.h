#pragma once
#include <./core/math/float3.h>
#include <./raytracing/ray.h>

namespace CRT
{
	struct AABB
	{
		float3 Min;
		float3 Max;

		bool Intersects(const Ray& ray) const
		{
			float3 fractionalDirection = 1.0f / ray.D;
			float3 tMin = (Min - ray.O) * fractionalDirection;
			float3 tMax = (Max - ray.O) * fractionalDirection;

			float tNearest = std::max({ std::min(tMin.x, tMax.x), std::min(tMin.y, tMax.y),
				std::min(tMin.z, tMax.z) });
			float tFarthest = std::min({ std::max(tMin.x, tMax.x), std::max(tMin.y, tMax.y),
				std::max(tMin.z, tMax.z) });

			return tFarthest >= 0.0f && tFarthest >= tNearest;
		}

		float GetSurfaceArea() const
		{
			float3 dimensions = (Max - Min).Abs();
			return 2.0f * (dimensions.x * dimensions.y + dimensions.y * dimensions.z +
				dimensions.z * dimensions.x);
		}
	};
}
