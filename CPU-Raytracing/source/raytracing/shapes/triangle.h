#pragma once
#include "./raytracing/shapes/shape.h"
#include "./raytracing/aabb.h"

namespace CRT
{
	class Material;


	// Alpha corresponds to I coordinate, beta to K coordinate and 
	// gamma to J
	// Note: order is deliberate, do not modify
	enum class EGridChange : uint8_t
	{
		IPlus   = 0,
		JMin    = 1,
		KPlus   = 2, 
		IMin    = 3,
		JPlus   = 4,
		KMin    = 5
	};

	EGridChange InvertGridchange(EGridChange _change);

	struct Cell
	{
		int32_t i, j, k;

		static Cell FromBarycentric(float3 _baryCentric, float _tesselation);
		bool IsUpperTriangle(uint32_t _tesselation) const;
		bool operator==(const Cell& other) const;
	};
	
	class Triangle final : public Shape
	{
	public:
		Triangle();
		Triangle(float3 _v0, float3 _v1, float3 _v2, float2 _u0, float2 _u1, float2 _u2, float3 _n0, float3 _n1, float3 _n2);

		virtual bool Intersect(Ray _r, Manifest& _m) const override;
		bool IntersectDisplacedNaive(Ray _r, Manifest& _m, const Texture* _heightmap) const;
		bool IntersectDisplaced(Ray _r, Manifest& _m, const Texture* _heightmap) const;

		bool IntersectTriangularSide(Ray _r, Triangle _tr, float _m, float& t0, float& t1, float3& inter0, float3& inter1, float3& _bary0, float3& _bary1, EGridChange& _startChange, float _tesselation) const;
		Triangle GetCell(float3 _bary, unsigned _tesselation) const;
		bool InitializeDisplaced(Ray _r, Cell& _start, Cell& _stop, EGridChange& _startChange) const;
		void SwapIntersection(float3& _inter0, float& _t0, float3& _bary0, float3& _inter1, float& _t1, float3 _bary1) const;
		bool IntersectSidePatch(Ray _r, float3 _p0, float3 _p1, float3 _n0, float3 _n1, float _m, float& _t, float3 _intersectionPoint) const;

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

		float3 GetCentroid() const;
		AABB GetBounds() const;
		AABB GetDisplacedBounds(float _maxHeight) const;
	};
}
