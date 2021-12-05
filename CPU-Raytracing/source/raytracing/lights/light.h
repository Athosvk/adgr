#pragma once
#include <./raytracing/ray.h>

namespace CRT
{
	class Manifest;

	struct ShadowRay
	{
		Ray Ray;
		float MaxT = 0.0f;
	};

	struct Light
	{
		virtual ~Light() {};
		virtual ShadowRay ConstructShadowRay(const Manifest& _manifest) const = 0;
		virtual float GetLightContribution(const Manifest& _manifest) const = 0;
	protected:
		float3 OffsetOrigin(const Manifest& _manifest) const;
	};
}
