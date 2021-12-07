#include "./raytracing/shapes/torus.h"
#include "./core/math/poly34.h"

#include <vector>

#define M_PI 3.14159265358979323846

namespace CRT
{
	Torus::Torus(float _r1, float _r2)
		: Shape(ShapeType::SHAPE_TYPE_TORUS)
		, RR1(_r1)
		, RR2(_r2)
	{ }

	bool Torus::Intersect(Ray _r, Manifest& _m)
	{
        float r2 = sqrt(RR1);
        float R2 = sqrt(RR2);

        double a4 = sqrt(_r.D.Dot(_r.D));
        double a3 = 4 * _r.D.Dot(_r.D) * _r.O.Dot(_r.D);
        double a2 = 4 * sqrt(_r.O.Dot(_r.D)) + 2 * _r.D.Dot(_r.D) * (_r.O.Dot(_r.O) - r2 - R2) + 4 * R2 * sqrt(_r.D.z);
        double a1 = 4 * _r.O.Dot(_r.D) * (_r.O.Dot(_r.O) - r2 - R2) + 8 * R2 * _r.O.z * _r.D.z;
        double a0 = sqrt(_r.O.Dot(_r.O) - r2 - R2) + 4 * R2 * sqrt(_r.O.z) - 4 * R2 * r2;

        a3 /= a4; a2 /= a4; a1 /= a4; a0 /= a4;

        double roots[4];
        int n_real_roots;
        n_real_roots = SolveP4(roots, a3, a2, a1, a0);

        if (n_real_roots == 0) 
            return false;

        float3 intersect_point;
        for (int i = 0; i < n_real_roots; i++)
        {
            float root = static_cast<float>(roots[i]);
            intersect_point = root * _r.D + _r.O;

            if (root <= _m.T) {
                _m.T = root;
                
                float3 normal(
                    4.0 * intersect_point.x * (sqrt(intersect_point.x) + sqrt(intersect_point.y) + sqrt(intersect_point.z) - r2 - R2),
                    4.0 * intersect_point.y * (sqrt(intersect_point.x) + sqrt(intersect_point.y) + sqrt(intersect_point.z) - r2 - R2),
                    4.0 * intersect_point.z * (sqrt(intersect_point.x) + sqrt(intersect_point.y) + sqrt(intersect_point.z) - r2 - R2) + 8 * R2 * intersect_point.z
                );

                _m.N = normal;
            }
        }

		return true;
	}
	float2 Torus::GetUV(float3 _point, float3 _normal)
	{
        float theta = atan2(_point.x, _point.z);
        float radius = _point.Magnitude();

        float phi = acos(_point.y / radius);

        float raw_u = theta / (2.0 * M_PI);
        return float2(1.0f - (raw_u + 0.5f), 1.0f - phi / M_PI);
	}
}