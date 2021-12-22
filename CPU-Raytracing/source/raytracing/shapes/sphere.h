#pragma once
#include "./raytracing/shapes/shape.h"

namespace CRT
{
	class Sphere final : public Shape
	{
	public:
		Sphere(float3 _position, float _radius);

		virtual bool Intersect(Ray _r, Manifest& _m) const override;
	private:
		bool IntersectOuter(Ray _r, Manifest& _m) const;
		bool IntersectInner(Ray _r, Manifest& _m) const;
		virtual float2 GetUV(float3 _point, float3 _normal) const override;

		float  Radius2;
		float3 Position;
	};
}