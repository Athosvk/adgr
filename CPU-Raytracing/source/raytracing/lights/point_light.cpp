#include <./raytracing/lights/point_light.h>
#include <./raytracing/manifest.h>

namespace CRT
{
	Ray PointLight::ConstructShadowRay(const Manifest& manifest) const
	{
		return { manifest.IntersectionPoint, manifest.IntersectionPoint - Position };
	}
}