#pragma once
#include "./raytracing/shapes/shape.h"

namespace CRT
{
	class Torus final : public Shape
	{
	public:
		Torus(float _r1, float _r2);

		virtual bool Intersect(Ray _r, Manifest& _m) override;

	private:
		virtual float2 GetUV(float3 _point, float3 _normal) override;

		float RR1;
		float RR2;
	};
}