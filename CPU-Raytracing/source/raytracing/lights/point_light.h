#pragma once
#include <./core/math/float3.h>
#include <./raytracing/lights/light.h>

namespace CRT
{
	struct PointLight : public Light
	{
		float3 Position;
		float Intensity;
		float3 Attenuation;

		virtual Ray ConstructShadowRay(const Manifest& manifest) const override;
	};
}
