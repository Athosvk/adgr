#include "directional_light.h"

#include "./raytracing/manifest.h"

namespace CRT
{
	DirectionalLight::DirectionalLight(float3 _direction, float _intensity, float3 _attenuation) : 
		Direction(_direction),
		Intensity(_intensity),
		Attenuation(_attenuation)
	{
	}

	Ray DirectionalLight::ConstructShadowRay(const Manifest& manifest) const
	{
		return Ray(manifest.IntersectionPoint, -Direction);
	}
}
