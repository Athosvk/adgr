#include <./raytracing/lights/point_light.h>
#include <./raytracing/manifest.h>
#include <./core/math/trigonometry.h>

#include <algorithm>

namespace CRT
{
	PointLight::PointLight(float3 _position, float _intensity, float3 _attenuation) : 
		Position(_position),
		Intensity(_intensity),
		Attenuation(_attenuation)
	{
	}

	ShadowRay PointLight::ConstructShadowRay(const Manifest& _manifest) const
	{
		auto offsetOrigin = OffsetOrigin(_manifest);
		auto toLight = Position - offsetOrigin;
		auto distanceToLight = toLight.Magnitude();
		Ray ray { offsetOrigin, toLight / distanceToLight };
		return { ray, distanceToLight };
	}
	
	float3 PointLight::GetLightContribution(const Manifest& manifest) const
	{
		float3 lightDirection = Position - manifest.IntersectionPoint;
		float distance = lightDirection.MagnitudeSquared();
		// Light contribution scales inversely with the distance to the light, based on the area
		// of the sphere containing the lit point
		float contribution = Intensity / (4 * Pi<float>() * distance * distance);
		return std::max(0.0f, manifest.ShadingNormal.Dot(lightDirection.Normalize())) * contribution * Attenuation;
	}
}