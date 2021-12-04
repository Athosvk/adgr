#include "directional_light.h"

#include <./raytracing/manifest.h>
#include <algorithm>
#include <limits>

namespace CRT
{
	DirectionalLight::DirectionalLight(float3 _direction, float _intensity, float3 _attenuation) : 
		Direction(_direction),
		Intensity(_intensity),
		Attenuation(_attenuation)
	{
	}

	ShadowRay DirectionalLight::ConstructShadowRay(const Manifest& _manifest) const
	{
		return { Ray(OffsetOrigin(_manifest), -Direction), std::numeric_limits<float>::infinity() };
	}

	float DirectionalLight::GetLightContribution(const Manifest& _manifest) const
	{
		return std::max(0.0f, _manifest.N.Dot(-Direction)) * Intensity;
	}
}
