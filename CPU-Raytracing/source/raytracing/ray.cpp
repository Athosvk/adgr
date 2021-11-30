#include "./raytracing/ray.h"

namespace CRT
{
	Ray Ray::Reflect(float3 _d, float3 _n, float3 _o)
	{
		float3 r = _d - 2 * (_d.Dot(_n)) * _n;
		return Ray(_o, r);
	}
}
