#pragma once
#include <./core/math/float3.h>
#include <./raytracing/ray.h>

#include <algorithm>

namespace CRT
{
	struct AABB
	{
		float3 Min = float3::Zero();
		float3 Max = float3::Zero();

		static AABB NegativeBox();

		AABB Extend(AABB other);
		AABB Extend(float3 point);

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

		bool Intersects(const RayPacket& ray, int id) const
		{
			float3 fractionalDirection = 1.0f / ray.D[id];
			float3 tMin = (Min - ray.O) * fractionalDirection;
			float3 tMax = (Max - ray.O) * fractionalDirection;

			float tNearest = std::max({ std::min(tMin.x, tMax.x), std::min(tMin.y, tMax.y),
				std::min(tMin.z, tMax.z) });
			float tFarthest = std::min({ std::max(tMin.x, tMax.x), std::max(tMin.y, tMax.y),
				std::max(tMin.z, tMax.z) });

			return tFarthest >= 0.0f && tFarthest >= tNearest;
		}

		bool IntersectFrustum(const RayPacket& ray) const
		{
			for (int i = 0; i < 4; i++)
			{
				float3 n = ray.P[i];

				float3 c = (Max + Min) * 0.5f;
				float3 e = Max - c;

				float r = e.x * std::abs(n.x) + e.y * std::abs(n.y) + e.z * std::abs(n.z);
				float s = n.Dot(c) - (ray.DD[i] - 150.0f);

				bool ret = std::abs(s) <= r;
				if (ret == true)
					return true;
			}
			return false;
		}

		int FindFirstActive(const RayPacket& ray, int _first) const
		{
			for (int i = _first + 1; i < 16 * 16; i++)
			{
				if (Intersects(ray, i))
					return i;
			}

			return -1;
		}

		bool IntersectPacket(const RayPacket& ray, int& _first) const
		{
			if (!Intersects(ray, _first))
			{
				if (IntersectFrustum(ray))
					return false;

				int f = FindFirstActive(ray, _first);
				if (f == -1)
					return false;
				else
				{
					_first = f;
					return true;
				}
			}
			return true;
		}

		float GetSurfaceArea() const
		{
			float3 dimensions = GetDimensions();
			return 2.0f * (dimensions.x * dimensions.y + dimensions.y * dimensions.z +
				dimensions.z * dimensions.x);
		}

		float3 GetDimensions() const
		{
			return (Max - Min).Abs();
		}
	};
}
