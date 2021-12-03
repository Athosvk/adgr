#pragma once
#include "./core/math/float3.h"

namespace CRT
{
	struct DirectionalLight : public Light
	{
		float3 Direction;
		float Intensity = 1.0f;
		float3 Attenuation;
	};
}
