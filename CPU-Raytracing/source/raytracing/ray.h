#pragma once
#include "./core/math/float3.h"
#include <glm/mat4x4.hpp>

namespace CRT
{
	class Ray
	{
	public:
		Ray(float3 _o, float3 _d)
			: O(_o)
			, D(_d)
		{ }

		float3 Sample(float t) const;
		Ray Reflect(float3 _n);

		Ray Transform(glm::mat4x4& _matrix);

		float3 O;
		float3 D;
	};
}
