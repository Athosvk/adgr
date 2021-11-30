#include "./raytracing/shapes/sphere.h"

#define M_PI 3.14159265358979323846

namespace CRT
{
	Sphere::Sphere(float3 _position, float _radius)
		: Shape(ShapeType::SHAPE_TYPE_SPHERE)
		, Position(_position)
		, Radius2(_radius* _radius)
	{ }

	bool Sphere::Intersect(Ray _r, Manifest& _m)
	{
		float3 C = Position - _r.O;
		float t = C.Dot(_r.D);
		float3 Q = C - t * _r.D;
		float p2 = Q.Dot(Q);
		if (p2 > Radius2)
			return false;

		t -= sqrt(Radius2 - p2);
		if (t > 0.0f && t < _m.T)
		{
			_m.T = t;
			float3 p = (_r.O + (_r.D * t));
			_m.N = (p - Position).Normalize();

			_m.UV = GetUV(p - Position, _m.N);
			return true;
		}

		return false;
	}
	float2 Sphere::GetUV(float3 _point, float3 _normal)
	{
		float theta = atan2(_point.x, _point.z);
		float radius = _point.Magnitude();

		float phi = acos(_point.y / radius);

		float raw_u = theta / (2.0 * M_PI);
		return float2(1.0f - (raw_u + 0.5f), 1.0f - phi / M_PI);
	}
}