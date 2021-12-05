#include "./raytracing/ray.h"

namespace CRT
{
	Ray Ray::Reflect(float3 _n)
	{
		float3 r = D - 2 * (D.Dot(_n)) * _n;
		return Ray(O, r);
	}

	float3 Ray::Sample(float _t) const
	{
		return O + D * _t;
	}
}
