#pragma once
#include <./core/math/float3.h>
#include <./raytracing/lights/light.h>

namespace CRT
{
	struct SpotLight : public Light
	{
		SpotLight(float3 _position, float3 _direction, float _cutoff, float _outerCuttoff, float _intensity, float3 _attenuation);

		virtual ShadowRay ConstructShadowRay(const Manifest& _manifest) const override;
		virtual float3 GetLightContribution(const Manifest& _manifest) const override;

		float3 Position;
		float3 Direction;
		float Cutoff;
		float OuterCutoff;
		float Intensity = 1.0f;
		float3 Attenuation = float3::One();
	};
}
