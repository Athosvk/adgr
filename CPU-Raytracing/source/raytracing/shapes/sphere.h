#pragma once
#include "./raytracing/shapes/shape.h"

namespace CRT
{
	class Sphere final : public Shape
	{
	public:
		Sphere(float3 _position, float _radius);

		virtual bool Intersect(Ray _r, Manifest& _m) override;

		float  Radius2;
		float3 Position;
	};
}