#pragma once
#include "./raytracing/shapes/shape.h"

namespace CRT
{
	class Triangle final : public Shape
	{
	public:
		Triangle();
		Triangle(float3 _v0, float3 _v1, float3 _v2, float3 _n);

		virtual bool Intersect(Ray _r, Manifest& _m) override;

		float3 v0;
		float3 v1;
		float3 v2;

		float2 u0;
		float2 u1;
		float2 u2;

		float3 N;
	private:
		float3 Barycentric(float3 _point);

		virtual float2 GetUV(float3 _point, float3 _normal) override;
	};
}