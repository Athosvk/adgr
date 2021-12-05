#include "light.h"
#include <./raytracing/manifest.h>

namespace CRT
{
	float3 Light::OffsetOrigin(const Manifest& _manifest) const
	{
		// Add bias for shadow rays, shifted along surface normal to guarantee self-intersection doesn't happen
		const float bias = 1e-4f;
		return _manifest.IntersectionPoint + _manifest.N * bias;
	}
}
