#include "plane.h"

namespace CRT
{
	Plane::Plane(float3 _point, float3 _normal)
		: Shape(ShapeType::SHAPE_TYPE_PLANE)
		, Point(_point)
		, Normal(_normal) 
	{ }

	bool Plane::Intersect(Ray _r, Manifest& _m)
	{
		float cosTheta = Normal.Dot(_r.D);
		// Backface check; if plane normal faces opposite direction of 
		// ray direction, the normal is facing the ray so it's a front face.
		if (-cosTheta > 1e-6)
		{
			float3 p0l0 = Point - _r.O;
			float t = p0l0.Dot(Normal) / cosTheta;

			if (t >= 0.0f)
			{
				_m.T = t;
				_m.N = Normal;
				return true;
			}
			return false;
		}
		return false;
	}

}