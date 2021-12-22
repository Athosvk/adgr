#include "plane.h"
#include <cmath>

namespace CRT
{
	Plane::Plane(float3 _point, float3 _normal)
		: Shape(ShapeType::SHAPE_TYPE_PLANE)
		, Point(_point)
		, Normal(_normal) 
	{ }

	bool Plane::Intersect(Ray _r, Manifest& _m) const
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
				_m.IntersectionPoint = _r.Sample(t);
				_m.UV = GetUV(_r.O+_r.D*_m.T, _m.N);
				return true;
			}
			return false;
		}
		return false;
	}

	float2 Plane::GetUV(float3 _point, float3 _normal) const
	{
		if (_point.x > 1.0f)
			int z = 0;

		float3 a = _normal.Cross(float3(1.0f, 0.0f, 0.0f));
		float3 b = _normal.Cross(float3(0.0f, 1.0f, 0.0f));
		float3 c = _normal.Cross(float3(0.0f, 0.0f, 1.0f));

		float3 max_ab = a.Dot(a) < b.Dot(b) ? b : a;

		float3 u = (max_ab.Dot(max_ab) < c.Dot(c) ? c : max_ab).Normalize();
		float3 v = _normal.Cross(u);

		float d;
		return float2(std::modf(fabs(u.Dot(_point)), &d), std::modf(fabs(v.Dot(_point)), &d));
	}

}