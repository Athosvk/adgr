#pragma once
#include "./core/math/float3.h"

namespace CRT
{
	class Ray
	{
	public:
		Ray(float3 _o, float3 _d)
			: O(_o)
			, D(_d)
		{ }

		float3 O;
		float3 D;
	};
}