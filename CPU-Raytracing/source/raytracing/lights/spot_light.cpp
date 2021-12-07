#include "./raytracing/lights/spot_light.h"
#include <./raytracing/manifest.h>
#include <./core/math/trigonometry.h>

#include <algorithm>

namespace CRT
{
	SpotLight::SpotLight(float3 _position, float3 _direction, float _cutoff, float _outerCuttoff, float _intensity, float3 _attenuation)
		: Position(_position)
		, Direction(_direction)
		, Cutoff(_cutoff)
		, OuterCutoff(_outerCuttoff)
		, Intensity(_intensity)
		, Attenuation(_attenuation)
	{ }

	ShadowRay SpotLight::ConstructShadowRay(const Manifest& _manifest) const
	{
		auto offsetOrigin = OffsetOrigin(_manifest);
		auto toLight = Position - offsetOrigin;
		auto distanceToLight = toLight.Magnitude();
		Ray ray{ offsetOrigin, toLight / distanceToLight };
		return { ray, distanceToLight };
	}

	float3 SpotLight::GetLightContribution(const Manifest& _manifest) const
	{
		float3 lightDirection = (Position - _manifest.IntersectionPoint).Normalize();
		float theta = lightDirection.Dot(-Direction);
		float epsilon = (Cutoff - OuterCutoff);
		float intensity = std::clamp((theta - OuterCutoff) / epsilon, 0.0f, 1.0f);

		float distance = lightDirection.MagnitudeSquared();
		float contribution = Intensity / (4 * Pi<float>() * distance * distance);

		return std::max(0.0f, _manifest.N.Dot(lightDirection.Normalize())) * Attenuation * intensity;
	}
}