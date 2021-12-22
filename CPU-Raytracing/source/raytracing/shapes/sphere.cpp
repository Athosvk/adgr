#include "./raytracing/shapes/sphere.h"

#include "./core/math/trigonometry.h"

namespace CRT
{
	Sphere::Sphere(float3 _position, float _radius)
		: Shape(ShapeType::SHAPE_TYPE_SPHERE)
		, Position(_position)
		, Radius2(_radius* _radius)
	{ }

	bool Sphere::Intersect(Ray _r, Manifest& _m) const
	{
		// Ray originates from inside the sphere
		if (Position.DistanceSquared(_r.O) <= Radius2)
		{
			return IntersectInner(_r, _m);
		}
		return IntersectOuter(_r, _m);
	}

	bool Sphere::IntersectOuter(Ray _r, Manifest& _m) const
	{
		float3 C = Position - _r.O;
		float t = C.Dot(_r.D);
		float3 Q = C - t * _r.D;
		float p2 = Q.Dot(Q);
		if (p2 > Radius2)
			return false;

		t -= sqrtf(Radius2 - p2);
		if (t > 0.0f)
		{
			_m.T = t;
			float3 p = (_r.O + (_r.D * t));
			_m.N = (p - Position).Normalize();
			_m.IntersectionPoint = p;

			_m.UV = GetUV(p - Position, _m.N);
			return true;
		}
		return false;
	}

	bool Sphere::IntersectInner(Ray _r, Manifest& _m) const
	{
		float3 toSphere = Position - _r.O;
		
		float tca = toSphere.Dot(_r.D);

		// Sphere is behind the ray
		if (tca < 0)
		{
			return false;
		}
		float d2 = toSphere.Dot(toSphere) - tca * tca;
		if (d2 > Radius2)
		{
			return false;
		}
		float thc = sqrtf(Radius2 - d2);
		float t0 = tca - thc;
		float t1 = tca + thc;

		// I.e. there are no solutions
		if (t0 < 0 && t1 < 0)
		{
			return false;
		}
		// Take the smallest non-negative value of t, which is the first intersection
		if (t0 > t1 || t0 < 0) 
			std::swap(t0, t1);
		_m.T = t0;
		_m.IntersectionPoint = _r.Sample(_m.T);
		_m.N = (_m.IntersectionPoint - Position).Normalize();
		_m.UV = GetUV(_m.IntersectionPoint - Position, _m.N);
		return true;
	}

	float2 Sphere::GetUV(float3 _point, float3 _normal) const
	{
		float theta = atan2f(_point.x, _point.z);
		float radius = _point.Magnitude();

		float phi = acosf(_point.y / radius);

		float raw_u = theta / (2.0f * Pi<float>());
		return float2(1.0f - (raw_u + 0.5f), 1.0f - phi / Pi<float>());
	}
}