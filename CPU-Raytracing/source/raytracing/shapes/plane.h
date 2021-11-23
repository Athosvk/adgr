#pragma once
#include "./raytracing/shapes/shape.h"

namespace CRT
{
	class Plane final : public Shape
	{
	public:
		Plane(float3 _point, float3 _normal);

		virtual bool Intersect(Ray _r, Manifest& _m) override;

		float3 Point;
		float3 Normal;
	};
}