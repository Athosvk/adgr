#pragma once
#include "./raytracing/shapes/shape.h"

namespace CRT
{
	class Sphere final : public Shape
	{
	public:
		Sphere(float3 _position, float _radius);

		virtual bool Intersect(Ray _r, Manifest& _m) override;
	private:
		bool IntersectOuter(Ray _r, Manifest& _m);
		bool IntersectInner(Ray _r, Manifest& _m);
		virtual float2 GetUV(float3 _point, float3 _normal) override;

		float  Radius2;
		float3 Position;
	};
}