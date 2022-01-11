#pragma once
#include "./raytracing/shapes/shape.h"

namespace CRT
{
	class Material;

	class Triangle final : public Shape
	{
	public:
		Triangle();
		Triangle(float3 _v0, float3 _v1, float3 _v2, float2 _u0, float2 _u1, float2 _u2, float3 _n0, float3 _n1, float3 _n2);

		virtual bool Intersect(Ray _r, Manifest& _m) const override;
		void Intersect(const RayPacket& ray, TraversalResultPacket& _result, int _first, int _id);

		float2 GetUV(float3 _point, float3 _normal) const;

		float3 V0;
		float3 V1;
		float3 V2;

		float2 u0;
		float2 u1;
		float2 u2;

		float3 N0;
		float3 N1;
		float3 N2;

		float GetSurfaceArea() const;
		float3 GetCentroid() const;
	};
}
