#pragma once
#include <./raytracing/ray.h>

namespace CRT
{
	class Manifest;

	struct Light
	{
		virtual Ray ConstructShadowRay(const Manifest& manifest) const = 0;
	};
}
