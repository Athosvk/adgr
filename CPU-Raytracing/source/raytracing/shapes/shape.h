#pragma once
#include "./core/math/float2.h"
#include "./raytracing/ray.h"
#include "./raytracing/manifest.h"

namespace CRT
{
	enum ShapeType
	{
		SHAPE_TYPE_NONE     = 0x00,
		SHAPE_TYPE_SPHERE   = 0x01,
		SHAPE_TYPE_PLANE    = 0x02,
		SHAPE_TYPE_TRIANGLE = 0x03,
		SHAPE_TYPE_TORUS    = 0x04,
	};

	class Shape
	{
	public:
		Shape(ShapeType _type)
			: m_Type(_type)
		{ }

		virtual bool Intersect(Ray _r, Manifest& _m) const = 0;

		// Getters
		inline ShapeType GetType() const { return m_Type; }

	protected:
		virtual float2 GetUV(float3 _point, float3 _normal) const = 0;

		ShapeType m_Type;
	};
}