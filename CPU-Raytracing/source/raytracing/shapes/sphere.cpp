#include "./raytracing/shapes/sphere.h"

#define M_PI 3.14159265358979323846

namespace CRT
{
	static void get_sphere_uv(const float3& p, float& u, float& v) {
		float theta = atan2(p.x, p.z);
		float radius = p.Magnitude();

		float phi = acos(p.y / radius);

		float raw_u = theta / (2.0 * M_PI);
		u = 1.0f - (raw_u + 0.5f);
		v = 1.0f - phi / M_PI;
	}

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

			float u, v;
			get_sphere_uv((p - Position), u, v);
			_m.UV = float2(u, v);
			return true;
		}

		return false;
	}
}