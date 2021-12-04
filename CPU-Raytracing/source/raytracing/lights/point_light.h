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

		PointLight(float3 _position, float _intensity, float3 _attenuation);

		virtual ShadowRay ConstructShadowRay(const Manifest& _manifest) const override;
		virtual float GetLightContribution(const Manifest& _manifest) const override;
	};
}
