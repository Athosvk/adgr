#pragma once
#include "./core/math/float3.h"

#include "light.h"

namespace CRT
{
	struct DirectionalLight : public Light
	{
		DirectionalLight(float3 _direction, float _intensity, float3 _attenuation);
		DirectionalLight() = default;

		float3 Direction;
		float Intensity = 1.0f;
		float3 Attenuation;

		virtual ShadowRay ConstructShadowRay(const Manifest& _manifest) const override;

		virtual float GetLightContribution(const Manifest& _manifest) const override;
	};
}
