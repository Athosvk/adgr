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
		bool IntersectDisplaced(Ray _r, Manifest& _m, Texture* _heightmap) const;

		bool InitializeDisplaced(Ray _r, float3& _inter0, float3& _inter1, float3& _bary) const;
		void SwapIntersection(float3& _inter0, float& _t0, float3& _inter1, float& _t1) const;
		bool IntersectSidePLane(Ray _r, float3 _p0, float3 _p1, float3 _n0, float3 _n1, float3& _p, float& _t) const;

		void Barycentric(float3& _vertex, float3& _normal, float2& _uv, float3 _bary) const;
		void ComputeBaryCentric(float3& _bary, float3 _p) const;

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
