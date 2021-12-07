#include "./raytracing/shapes/triangle.h"

namespace CRT
{
    Triangle::Triangle()
        : Shape(ShapeType::SHAPE_TYPE_TRIANGLE)
    {  }

    Triangle::Triangle(float3 _v0, float3 _v1, float3 _v2, float3 _n)
		: Shape(ShapeType::SHAPE_TYPE_TRIANGLE)
        , v0(_v0)
        , v1(_v1)
        , v2(_v2)
        , N(_n)
	{ }

	bool Triangle::Intersect(Ray _r, Manifest & _m)
	{
        float3 v0v1 = v1 - v0;
        float3 v0v2 = v2 - v0;
        float3 pvec = _r.D.Cross(v0v2);
        float det = v0v1.Dot(pvec);
#ifdef CULLING 
        // if the determinant is negative the triangle is backfacing
        // if the determinant is close to 0, the ray misses the triangle
        if (det < kEpsilon) return false;
#else 
        // ray and triangle are parallel if det is close to 0
        if (fabs(det) < 0.001) return false;
#endif 
        float invDet = 1 / det;

        float3 tvec = _r.O - v0;
        float u = tvec.Dot(pvec) * invDet;
        if (u < 0 || u > 1) return false;

        float3 qvec = tvec.Cross(v0v1);
        float v = _r.D.Dot(qvec) * invDet;
        if (v < 0 || u + v > 1) return false;

        float t = v0v2.Dot(qvec) * invDet;
        if (t < _m.T)
        {
            _m.T = t;
            _m.UV = GetUV(_r.O+_r.D*t, N);
            _m.N  = N;
            return true;
        }

        return false;
	}

    float3 Triangle::Barycentric(float3 _point)
    {
        float3 v0 = v1 - v0, v1 = v2 - v0, v2 = _point - v0;
        float d00 = v0.Dot(v0);
        float d01 = v0.Dot(v1);
        float d11 = v1.Dot(v1);
        float d20 = v2.Dot(v0);
        float d21 = v2.Dot(v1);
        float denom = d00 * d11 - d01 * d01;
        
        float v = (d11 * d20 - d01 * d21) / denom;
        float w = (d00 * d21 - d01 * d20) / denom;
        float u = 1.0f - v - w;

        return float3(u, v, w);
    }

    float2 Triangle::GetUV(float3 _point, float3 _normal)
	{
        float3 barycentric = Barycentric(_point);
		return (barycentric.x * u0) + (barycentric.y * u1) + (barycentric.z * u2);
	}
}