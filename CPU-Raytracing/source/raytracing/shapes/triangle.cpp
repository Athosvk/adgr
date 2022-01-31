#include "./raytracing/shapes/triangle.h"
#include "./raytracing/shapes/plane.h"

#include <limits>

namespace CRT
{
    Triangle::Triangle()
        : Shape(ShapeType::SHAPE_TYPE_TRIANGLE)
    {  }

    Triangle::Triangle(float3 _v0, float3 _v1, float3 _v2, float2 _u0, float2 _u1, float2 _u2, float3 _n0, float3 _n1, float3 _n2)
        : Shape(ShapeType::SHAPE_TYPE_TRIANGLE)
        , V0(_v0)
        , V1(_v1)
        , V2(_v2)
        , u0(_u0)
        , u1(_u1)
        , u2(_u2)
        , N0(_n0)
        , N1(_n1)
        , N2(_n2)
    { }
    bool Triangle::Intersect(Ray _r, Manifest& _m) const
    {
        float3 v0v1 = V1 - V0;
        float3 v0v2 = V2 - V0;
        float3 pvec = _r.D.Cross(v0v2);
        float det = v0v1.Dot(pvec);
#ifdef CULLING 
        // if the determinant is negative the triangle is backfacing
        // if the determinant is close to 0, the ray misses the triangle
        if (det <= 0.0f) return false;
#else 
        // ray and triangle are parallel if det is close to 0
        if (det == 0.0f) return false;
#endif 
        float invDet = 1 / det;

        float3 tvec = _r.O - V0;
        float u = tvec.Dot(pvec) * invDet;
        if (u < 0 || u > 1) return false;

        float3 qvec = tvec.Cross(v0v1);
        float v = _r.D.Dot(qvec) * invDet;
        if (v < 0 || u + v > 1) return false;

        float t = qvec.Dot(v0v2) * invDet;
        if (t > 0.0f && t < _m.T)
        {
            float3 barycentric {1.0f - u - v, u, v };
            _m.IntersectionPoint = _r.Sample(t);
            _m.T = t;
            _m.SurfaceNormal = (V1 - V2).Cross(V1 - V0).Normalize();
            _m.UV = (barycentric.x * u0) + (barycentric.y * u1) + (barycentric.z * u2);
            _m.ShadingNormal =  (N0 * barycentric.x
                               + N1 * barycentric.y 
                               + N2 * barycentric.z).Normalize();
            return true;
        }

        return false;
    }

    bool Triangle::InitializeDisplaced(Ray _r, float3& _inter0, float3& _inter1, float3& _bary) const
    {
        float m = 1.0f;
        float tes = 4.0f;
        float3 inter0, inter1, bary;
        float t0 = FLT_MAX, t1 = FLT_MAX;
        {
            if (IntersectSidePLane(_r, V0, V1, N0, N1, inter1, t1))
            {
                if (t1 < t0)
                {
                    SwapIntersection(inter0, t0, inter1, t1);

                    float len = (V1 - V0).Magnitude();
                    float3 nx = (V1 - V0).Normalize();
                    float  nb = (inter0 - V0).Dot(nx) / len;

                    bary.x = 1.0f-nb;
                    bary.y = nb;
                    bary.z = 0.0f;
                }
            }
            if (IntersectSidePLane(_r, V1, V2, N1, N2, inter1, t1))
            {
                if (t1 < t0)
                {
                    SwapIntersection(inter0, t0, inter1, t1);
                    
                    float len = (V2 - V1).Magnitude();
                    float3 nx = (V2 - V1).Normalize();
                    float  nb = (inter0 - V1).Dot(nx) / len;

                    bary.x = 0.0f;
                    bary.y = 1.0f-nb;
                    bary.z = nb;
                }
            }
            if (IntersectSidePLane(_r, V2, V0, N2, N0, inter1, t1))
            {
                if (t1 < t0)
                {
                    SwapIntersection(inter0, t0, inter1, t1);
                    
                    float len = (V0 - V2).Magnitude();
                    float3 nx = (V0 - V2).Normalize();
                    float  nb = (inter0 - V2).Dot(nx) / len;

                    bary.x = nb;
                    bary.y = 0.0f;
                    bary.z = 1.0f-nb;
                }
            }

            Triangle tr(V0 + N0 * m, V1 + N1 * m, V2 + N2 * m, u0, u1, u2, N0, N1, N2);
            Manifest ma;
            if (tr.Intersect(_r, ma))
            {
                inter1 = ma.IntersectionPoint;
                t1 = ma.T;
                if (t1 < t0)
                {
                    SwapIntersection(inter0, t0, inter1, t1);
                    ComputeBaryCentric(bary, inter0);
                }
            }
            Triangle tr(V0 + N0 * -m, V1 + N1 * -m, V2 + N2 * -m, u0, u1, u2, N0, N1, N2);
            if (tr.Intersect(_r, ma))
            {
                inter1 = ma.IntersectionPoint;
                t1 = ma.T;
                if (t1 < t0)
                {
                    SwapIntersection(inter0, t0, inter1, t1);
                    ComputeBaryCentric(bary, inter0);
                }
            }
        }
        if (inter0 < FLT_MAX && inter1 < FLT_MAX)
        {
            _inter0 = inter0;
            _inter1 = inter1;
            _bary = float3(floor(bary.x * tes), floor(bary.y * tes), floor(bary.z * tes));
            return true;
        }
        return false;
    }

    void Triangle::SwapIntersection(float3& _inter0, float& _t0, float3& _inter1, float& _t1) const
    {
        if (_t0 < FLT_MAX)
        {
            std::swap(_t0, _t1);
            std::swap(_inter0.x, _inter1.x);
            std::swap(_inter0.y, _inter1.y);
            std::swap(_inter0.z, _inter1.z);
        }
        else
        {
            _t0 = _t1;
            _inter0 = _inter1;
        }
    }

    bool Triangle::IntersectSidePLane(Ray _r, float3 _p0, float3 _p1, float3 _n0, float3 _n1, float3& _p, float& _t) const
    {
        float3 n = (_p0 - _p1).Cross((_n0 + _n1).Normalize());
        float3 p = _p0;
        Plane pl(p, n);
        Manifest m;
        if (pl.Intersect(_r, m))
        {
            _p = m.IntersectionPoint;
            _t = m.T;
        }
    }

    void Triangle::Barycentric(float3& _vertex, float3& _normal, float2& _uv, float3 _bary) const
    {
        _vertex = V0 * _bary.x + V1 * _bary.y + V2 * _bary.z;
        _normal = (N0 * _bary.x + N1 * _bary.y + N2 * _bary.z).Normalize();
        _uv = u0 * _bary.x + u1 * _bary.y + u2 * _bary.z;
    }

    void Triangle::ComputeBaryCentric(float3& _bary, float3 _p) const
    {
        float3 v0 = V1 - V0;
        float3 v1 = V2 - V0;
        float3 v2 = _p - V0;

        float d00 = v0.Dot(v0);
        float d01 = v0.Dot(v1);
        float d11 = v1.Dot(v1);
        float d20 = v2.Dot(v0);
        float d21 = v2.Dot(v1);

        float denom = d00 * d11 - d01 * d01;
        _bary.x = (d11 * d20 - d01 * d21) / denom;
        _bary.y = (d00 * d21 - d01 * d20) / denom;
        _bary.z = 1.0f - _bary.x - _bary.y;
    }

    bool Triangle::IntersectDisplaced(Ray _r, Manifest& _m, Texture* _heightmap) const
    {
        // Initialization
       // InitializeDisplaced(_r);

      

        //(A*a + B*b + C*c) / (a + b + c)
        {
            // (0, 0, 1)
            float3 t0p0 = float3(0, 0, 1);
            float3 t0p1 = float3(0, 0.5, 0.5);
            float3 t0p2 = float3(0.5, 0, 0.5);

            float3 v0t0, n0t0; float2 u0t0;
            float3 v0t1, n0t1; float2 u0t1;
            float3 v0t2, n0t2; float2 u0t2;
            Barycentric(v0t0, n0t0, u0t0, t0p0);
            Barycentric(v0t1, n0t1, u0t1, t0p1);
            Barycentric(v0t2, n0t2, u0t2, t0p2);

            // WE ONLY CARE ABOUT THE GREY_SCALE VALUE
            float d0t0 = _heightmap->GetValue(u0t0).x;
            float d0t1 = _heightmap->GetValue(u0t1).x;
            float d0t2 = _heightmap->GetValue(u0t2).x;

            // Displaced VertexCoordinates
            float3 vn0t0 = v0t0 + (n0t0*d0t0);
            float3 vn0t1 = v0t1 + (n0t1*d0t1);
            float3 vn0t2 = v0t2 + (n0t2*d0t1);

            // (0, 0, 0)
            float3 t1p0 = t0p2;
            float3 t1p1 = t0p1;
            float3 t1p2 = float3(0.5, 0.5, 0);

            float3 v1t0 = v0t2, n1t0 = n0t2; float2 u1t0 = u0t2;
            float3 v1t1 = v0t1, n1t1 = n0t1; float2 u1t1 = u0t1;
            float3 v1t2, n1t2; float2 u1t2;
            Barycentric(v1t2, n1t2, u1t2, t1p2);

            // WE ONLY CARE ABOUT THE GREY_SCALE VALUE
            float d1t0 = d0t2;
            float d1t1 = d0t1;
            float d1t2 = _heightmap->GetValue(u1t2).x;

            // Displaced VertexCoordinates
            float3 vn1t0 = vn0t2;
            float3 vn1t1 = vn0t1;
            float3 vn1t2 = v1t2 + (n1t2*d1t1);

            // (1, 0, 0)
            float3 t2p0 = t0p2;
            float3 t2p1 = t1p2;
            float3 t2p2 = float3(1, 0, 0);

            float3 v2t0 = v0t2, n2t0 = n0t2; float2 u2t0 = u0t2;
            float3 v2t1 = v1t2, n2t1 = n1t2; float2 u2t1 = u1t2;
            float3 v2t2, n2t2;  float2 u2t2;
            Barycentric(v2t2, n2t2, u2t2, t2p2);

            // WE ONLY CARE ABOUT THE GREY_SCALE VALUE
            float d2t0 = d0t2;
            float d2t1 = d1t2;
            float d2t2 = _heightmap->GetValue(u2t2).x;

            // Displaced VertexCoordinates
            float3 vn2t0 = vn0t2;
            float3 vn2t1 = vn1t2;
            float3 vn2t2 = v2t2 + (n2t2 * d2t1);

            // (0, 1, 0)
            float3 t3p0 = t0p1;
            float3 t3p1 = float3(0, 1, 0);
            float3 t3p2 = t1p2;

            float3 v3t0 = v0t1, n3t0 = n0t1; float2 u3t0 = u0t1;
            float3 v3t1, n3t1; float2 u3t1;
            float3 v3t2 = v1t2, n3t2 = n1t2; float2 u3t2 = u1t2;
            Barycentric(v3t1, n3t1, u3t1, t3p1);

            // WE ONLY CARE ABOUT THE GREY_SCALE VALUE
            float d3t0 = d0t1;
            float d3t1 = _heightmap->GetValue(u3t1).x;
            float d3t2 = d1t2;

            // Displaced VertexCoordinates
            float3 vn2t0 = vn0t1;
            float3 vn2t1 = v2t1 + (n2t1*d2t1);
            float3 vn2t2 = vn1t2;
        }

        return false;
    }

    void Triangle::Intersect(const RayPacket& ray, TraversalResultPacket& _result, int _first, int _id)
    {
        for (int i = _first; i < 16 * 16; i++)
        {
            float3 v0v1 = V1 - V0;
            float3 v0v2 = V2 - V0;
            float3 pvec = ray.D[i].Cross(v0v2);
            float det = v0v1.Dot(pvec);
#ifdef CULLING 
            // if the determinant is negative the triangle is backfacing
            // if the determinant is close to 0, the ray misses the triangle
            if (det < std::numeric_limits<float>::epsilon()) continue;
#else 
            // ray and triangle are parallel if det is close to 0
            if (fabs(det) < std::numeric_limits<float>::epsilon()) continue;
#endif 
            float invDet = 1 / det;

            float3 tvec = ray.O - V0;
            float u = tvec.Dot(pvec) * invDet;
            if (u < 0 || u > 1)continue;

            float3 qvec = tvec.Cross(v0v1);
            float v = ray.D[i].Dot(qvec) * invDet;
            if (v < 0 || u + v > 1) continue;

            float t = qvec.Dot(v0v2) * invDet;
            if (t > 0.0f && t < _result.T[i])
            {
                _result.T[i] = t;
                _result.ID[i] = _id;
            }
        }
    }

    float Triangle::GetSurfaceArea() const
    {
        return (V1 - V0).Cross(V2 - V0).Magnitude() * 0.5f;
    }

    float3 Triangle::GetCentroid() const
    {
        return (V0 + V1 + V2) / 3.0f;
    }
    float2 Triangle::GetUV(float3 _point, float3 _normal) const
    {
		// Calculated during intersection
        return float2{};
    }
}
