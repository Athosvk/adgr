#include "plane.h"

namespace CRT
{
	Plane::Plane(float3 _point, float3 _normal)
		: Shape(ShapeType::SHAPE_TYPE_PLANE)
		, Point(_point)
		, Normal(-_normal) // TODO:: Look into this minus...
	{ }

	bool Plane::Intersect(Ray _r, Manifest& _m)
	{
		float t = Normal.Dot(_r.D);
		if (t > 1e-6) {
			float3 p0l0 = Point - _r.O;
			t = p0l0.Dot(Normal) / t;
			return (t >= 0.0f);
		}
		return false;
	}

}