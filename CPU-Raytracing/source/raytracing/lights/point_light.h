#pragma once
#include <./core/math/float3.h>
#include <./raytracing/lights/light.h>

namespace CRT
{
	struct PointLight : public Light
	{
		PointLight(float3 _position, float _intensity, float3 _attenuation);

		virtual ShadowRay ConstructShadowRay(const Manifest& _manifest) const override;
		virtual float3 GetLightContribution(const Manifest& _manifest) const override;

		float3 Position;
		float Intensity = 1.0f;
		float3 Attenuation = float3::One();
	};
}
