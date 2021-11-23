#include "./raytracing/shapes/sphere.h"

namespace CRT
{
	Sphere::Sphere(float3 _position, float _radius)
		: Shape(ShapeType::SHAPE_TYPE_SPHERE)
		, Position(_position)
		, Radius2(_radius*_radius)
	{ }

	bool Sphere::Intersect(Ray _r, Manifest& _m)
	{
		float3 C = Position - _r.O;
		float t = C.Dot(_r.D);
		float3 Q = C - t * _r.D;
		float p2 = Q.Dot(Q);
		if (p2 > Radius2)
			return false;

		if (t > 0.0f && t < _m.T)
		{
			_m.T = t;
			float3 p = (_r.O + (_r.D*t));
			_m.N = (p - Position).Normalize();
			return true;
		}

		return false;
	}
}