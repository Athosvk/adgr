#pragma once
#include "./core/math/float3.h"

#include "light.h"

namespace CRT
{
	struct DirectionalLight : public Light
	{
		DirectionalLight(float3 _direction, float _intensity, float3 _attenuation);
		DirectionalLight() = default;

		virtual ShadowRay ConstructShadowRay(const Manifest& _manifest) const override;
		virtual float3 GetLightContribution(const Manifest& _manifest) const override;

		float3 Direction;
		float Intensity = 1.0f;
		float3 Attenuation = float3::One();
	};
}
