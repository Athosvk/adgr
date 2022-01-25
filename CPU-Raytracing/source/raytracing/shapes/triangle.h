#pragma once
#include "./raytracing/shapes/shape.h"
#include "./raytracing/aabb.h"

namespace CRT
{
	class Material;

	class Triangle
	{
	public:
		Triangle();
		Triangle(float3 _v0, float3 _v1, float3 _v2, float2 _u0, float2 _u1, float2 _u2, float3 _n0, float3 _n1, float3 _n2);

		bool Intersect(Ray _r, Manifest& _m) const;
		bool IntersectDisplaced(Ray _r, Manifest& _m, const Texture* _heightmap) const;
		
		void Barycentric(float3& _vertex, float3& _normal, float2& _uv, float3 _bary) const;
		
		void Intersect(const RayPacket& ray, TraversalResultPacket& _result, int _first, int _id);

		float3 V0;
		float3 V1;
		float3 V2;

		float2 u0;
		float2 u1;
		float2 u2;

		float3 N0;
		float3 N1;
		float3 N2;

		float3 GetCentroid() const;
		AABB GetBounds() const;
		AABB GetDisplacedBounds(float _maxHeight) const;
	};
}
