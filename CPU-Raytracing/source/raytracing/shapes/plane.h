#pragma once
#include "./raytracing/shapes/shape.h"

namespace CRT
{
	class Plane final : public Shape
	{
	public:
		Plane(float3 _point, float3 _normal);

		virtual bool Intersect(Ray _r, Manifest& _m) const override;
		bool IntersectLine(Ray _r, Manifest& _m);

	private:
		virtual float2 GetUV(float3 _point, float3 _normal) const override;

		float3 Point;
		float3 Normal;
	};
}
