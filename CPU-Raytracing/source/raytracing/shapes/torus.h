#pragma once
#include "./raytracing/shapes/shape.h"
#include <glm/mat4x4.hpp>

namespace CRT
{
	class Torus final : public Shape
	{
	public:
		Torus(float3 _pos, float _r1, float _r2);

		virtual bool Intersect(Ray _r, Manifest& _m) override;

	private:
		virtual float2 GetUV(float3 _point, float3 _normal) override;

		float3 Position;
		glm::mat4x4 invModel;
		glm::mat4x4 model;

		float RR1;
		float RR2;
	};
}
