#include "./raytracing/shapes/triangle.h"
#include "./raytracing/shapes/plane.h"

#include <limits>
#include <array>
#include <cstdint>

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
            float3 barycentric{ 1.0f - u - v, u, v };
            _m.IntersectionPoint = _r.Sample(t);
            _m.T = t;
            _m.SurfaceNormal = (V1 - V2).Cross(V1 - V0).Normalize();
            _m.UV = (barycentric.x * u0) + (barycentric.y * u1) + (barycentric.z * u2);
            _m.ShadingNormal = (N0 * barycentric.x
                + N1 * barycentric.y
                + N2 * barycentric.z).Normalize();
            return true;
        }

        return false;
    }

    bool Triangle::IntersectDisplacedNaive(Ray _r, Manifest& _m, const Texture* _heightmap) const
    {
        uint32_t numSubdivisions = 4;

        float delta = 1.0f / numSubdivisions;
        bool exitedGrid = false;
        
        Manifest nearest;
        int numTriangles = 0;
        for (int j = 0; j < numSubdivisions; j++)
        {
            for (int k = 0; k < numSubdivisions; k++)
            {
                for (int i = 0; i < numSubdivisions; i++)
                {
                    float2 uvV0, uvV1, uvV2;
                    // Upper left
                    uvV0 = float2(j * delta, (k + 1) * delta);
                    if (uvV0.x + uvV0.y > 1) continue;

                    // Lower right
                    uvV1 = float2((j + 1) * delta, k * delta);
                    if (uvV1.x + uvV1.y > 1) continue;
                    // Upper triangle test
                    if ((i + j + k) % 2 == 0)
                    {
                        // Upper right
                        uvV2 = float2((j + 1) * delta, (k + 1) * delta);
                    }
                    else
                    {
                        // Lower left
                        uvV2 = float2(j * delta, k * delta);
                    }
                    if (uvV2.x + uvV2.y > 1) continue;
                    Triangle microTriangle;
                    Barycentric(microTriangle.V0, microTriangle.N0, microTriangle.u0, float3(1.0 - uvV0.x - uvV0.y, uvV0.x, uvV0.y));
                    Barycentric(microTriangle.V1, microTriangle.N1, microTriangle.u1, float3(1.0 - uvV1.x - uvV1.y, uvV1.x, uvV1.y));
                    Barycentric(microTriangle.V2, microTriangle.N2, microTriangle.u2, float3(1.0 - uvV2.x - uvV2.y, uvV2.x, uvV2.y));

                    microTriangle.V0 += _heightmap->GetValue(microTriangle.u0).x * microTriangle.N0;
                    microTriangle.V1 += _heightmap->GetValue(microTriangle.u1).x * microTriangle.N1;
                    microTriangle.V2 += _heightmap->GetValue(microTriangle.u2).x * microTriangle.N2;

                    Manifest manifest;
                    if (microTriangle.Intersect(_r, manifest))
                    {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    bool Triangle::IntersectTriangularSide(Ray _r, Triangle _tr, float& t0, float& t1, float3& inter0, float3& inter1, float3& _bary, float _tesselation) const
    {
        Manifest ma;
		if (_tr.Intersect(_r, ma))
		{
			inter1 = ma.IntersectionPoint;
			t1 = ma.T;
			if (t1 < t0)
			{
				//SwapIntersection(inter0, t0, inter1, t1);
				ComputeBaryCentric(_bary, inter0);
			}

            Triangle cell = GetCell(_bary, _tesselation);

            // Determine the sideplane from which we entered the cell.
            // The side-plane is specific to upper triangles and lower triangles
            // If lower triangle, V0-V1 = iplus, V0-V2 = kplus, V1-V2 = jplus
            // If upper triangle, V0-V1 = imin, V0-V2 = kmin, V1-V2 = jmin
			float3 cellIntersectionPoint;
            float t;
			if (IntersectSidePLane(_r, cell.V0, cell.V1, _tr.N0, _tr.N1, cellIntersectionPoint, t))
			{
				float len = (_tr.V1 - _tr.V0).Magnitude();
				float3 nx = (_tr.V1 - _tr.V0).Normalize();
				float  nb = (inter0 - _tr.V0).Dot(nx) / len;

				_bary.x = 1.0f - nb;
				_bary.y = nb;
				_bary.z = 0.0f;
			}
			if (IntersectSidePLane(_r, cell.V1, cell.V2, _tr.N1, _tr.N2, cellIntersectionPoint, t))
			{
				float len = (_tr.V2 - _tr.V1).Magnitude();
				float3 nx = (_tr.V2 - _tr.V1).Normalize();
				float  nb = (inter0 - _tr.V1).Dot(nx) / len;

				_bary.x = 0.0f;
				_bary.y = 1.0f - nb;
				_bary.z = nb;
			}

			if (IntersectSidePLane(_r, cell.V0, cell.V2, _tr.N0, _tr.N2, cellIntersectionPoint, t))
			{
				float len = (_tr.V0 - _tr.V2).Magnitude();
				float3 nx = (_tr.V0 - _tr.V2).Normalize();
				float  nb = (inter0 - _tr.V2).Dot(nx) / len;

				_bary.x = 0.0f;
				_bary.y = 1.0f - nb;
				_bary.z = nb;
			}
            return true;
		}
    }

    Triangle Triangle::GetCell(float3 _bary, unsigned _tesselation) const
    {
        // _bary contains indices in order i, j, k
       _bary = float3(floor(_bary.x * _tesselation), floor(_bary.y * _tesselation), 
           floor(_bary.z * _tesselation));

        Triangle triangle;
        // if lower triangle
        // The "upperleft" vertex of the microtriangle
        float3 uvV0 = float3(_bary.x, _bary.y, _bary.z + 1) / _tesselation;
        triangle.V0 = uvV0.x * V0 + uvV0.y * V1 + uvV0.z * V2;

        // The "lowerright" vertex of the microtriangle
        float3 uvV1 = float3(_bary.x, _bary.y + 1, _bary.z) / _tesselation;
        triangle.V1 = uvV1.x * V0 + uvV1.y * V1 + uvV1.z * V2;

        // Every upper triangle is adjacent to lower triangles only. Therefore, every other
        // triangle is an upper triangle. (0,0,0) is an upper triangle in every tesselation
        // dividable by 2
        bool isUpperTriangle = unsigned(_bary.x + _bary.y + _bary.z) % 2 == 0 && _tesselation % 2 == 0;
        // The "lowerleft" or "upperright" vertex of the microtriangle, depending on wehther
        // this is the lower or upper triangle
        float3 uvV2 = (isUpperTriangle ?    float3(_bary.x, _bary.y + 1, _bary.z + 1) :
                                            float3(_bary.x, _bary.y, _bary.z))
                      / _tesselation;
        triangle.V2 = uvV2.x * V0 + uvV2.y * V1 + uvV2.z * V2;

        return triangle;
    }
    
    bool Triangle::InitializeDisplaced(Ray _r, Cell& _start, Cell& _stop, EGridChange& _startChange) const
    {
        float m = 1.0f;
        float tes = 4.0f;
        float3 inter0, inter1, bary0, bary1;
        float t0 = FLT_MAX, t1 = FLT_MAX;
        {
            if (IntersectSidePLane(_r, V0, V1, N0, N1, inter1, t1))
            {
                if (t1 < t0)
                {
                    SwapIntersection(inter0, t0, bary0, inter1, t1, bary1);

                    float len = (V1 - V0).Magnitude();
                    float3 nx = (V1 - V0).Normalize();
                    float  nb = (inter0 - V0).Dot(nx) / len;

                    bary0.x = 1.0f - nb;
                    bary0.y = nb;
                    bary0.z = 0.0f;
                    _startChange = EGridChange::KPlus;
                }
            }
            if (IntersectSidePLane(_r, V1, V2, N1, N2, inter1, t1))
            {
                if (t1 < t0)
                {
                    SwapIntersection(inter0, t0, bary0, inter1, t1, bary1);

                    float len = (V2 - V1).Magnitude();
                    float3 nx = (V2 - V1).Normalize();
                    float  nb = (inter0 - V1).Dot(nx) / len;

                    bary0.x = 0.0f;
                    bary0.y = 1.0f - nb;
                    bary0.z = nb;
                    _startChange = EGridChange::IPlus;
                }
            }
            if (IntersectSidePLane(_r, V2, V0, N2, N0, inter1, t1))
            {
                if (t1 < t0)
                {
                    SwapIntersection(inter0, t0, bary0, inter1, t1, bary1);

                    float len = (V0 - V2).Magnitude();
                    float3 nx = (V0 - V2).Normalize();
                    float  nb = (inter0 - V2).Dot(nx) / len;

                    bary0.x = nb;
                    bary0.y = 0.0f;
                    bary0.z = 1.0f - nb;
                    _startChange = EGridChange::JPlus;
                }
            }

            //Triangle tr(V0 + N0 * m, V1 + N1 * m, V2 + N2 * m, u0, u1, u2, N0, N1, N2);
            //Manifest ma;
            //if (tr.Intersect(_r, ma))
            //{
            //    inter1 = ma.IntersectionPoint;
            //    t1 = ma.T;
            //    if (t1 < t0)
            //    {
            //        SwapIntersection(inter0, t0, inter1, t1);
            //        // Use from intersection?
            //        ComputeBaryCentric(bary, inter0);
            //        
            //        float3 cell = float3(floor(bary.x * tes), floor(bary.y * tes), floor(bary.z * tes));
            //    }
            //}
            //tr = Triangle(V0 + N0 * -m, V1 + N1 * -m, V2 + N2 * -m, u0, u1, u2, N0, N1, N2);
        }
        if (inter0 < FLT_MAX && inter1 < FLT_MAX)
        {
            _start = Cell{ int32_t(bary0.x * tes), int32_t(bary0.y * tes), int32_t(bary0.z * tes) };
            _stop = Cell{ int32_t(bary1.x * tes), int32_t(bary1.y * tes), int32_t(bary1.z * tes) };
            return true;
        }
        return false;
    }

    void Triangle::SwapIntersection(float3& _inter0, float& _t0, float3& _bary0, float3& _inter1, float& _t1, float3 _bary1) const
    {
        if (_t0 < FLT_MAX)
        {
            std::swap(_t0, _t1);
            std::swap(_inter0.x, _inter1.x);
            std::swap(_inter0.y, _inter1.y);
            std::swap(_inter0.z, _inter1.z);
            std::swap(_bary0.x, _bary1.x);
            std::swap(_bary0.y, _bary1.y);
            std::swap(_bary0.z, _bary1.z);
        }
        else
        {
            _t0 = _t1;
            _inter0 = _inter1;
            _bary0 = _bary1;
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
            return true;
        }
        return false;
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

    bool Triangle::IntersectDisplaced(Ray _r, Manifest& _m, const Texture* _heightmap) const
    {
        return IntersectDisplacedNaive(_r, _m, _heightmap);

        Cell currentCell;
        Cell stopCell;
        EGridChange change;
        if (!InitializeDisplaced(_r, currentCell, stopCell, change))
        {
            return false;
        }
        Manifest nearest;

        uint32_t numSubdivisions = 2;
        std::array<float2, 3> barycentricPositions;

        Triangle microTriangle; 
        float delta = 1.0f / numSubdivisions;
        bool exitedGrid = false;
        
		Barycentric(microTriangle.V0, microTriangle.N0, microTriangle.u0, barycentricPositions[0]);
		Barycentric(microTriangle.V1, microTriangle.N1, microTriangle.u1, barycentricPositions[1]);

        microTriangle.V0 += _heightmap->GetValue(barycentricPositions[0]).x * microTriangle.N0;
        microTriangle.V1 += _heightmap->GetValue(barycentricPositions[1]).x * microTriangle.N1;
        int iteration = 0;
        int maxIterations = numSubdivisions * numSubdivisions;
        
        while(!exitedGrid && ++iteration <= maxIterations)
        {
            Barycentric(microTriangle.V2, microTriangle.N2, microTriangle.u2, barycentricPositions[2]);
            microTriangle.V2 += _heightmap->GetValue(barycentricPositions[2]).x * microTriangle.N2;

            if (microTriangle.Intersect(_r, nearest))
            {
                _m = nearest;
                return true;
            }
            if (currentCell == stopCell)
            {
                exitedGrid = true;
            }

            // First get the vector perpendicular to the V2 normal and the direction towards the ray origin.
            // If the incoming ray projects to the right of V2, which is where this cross product should be aimed to,
            // then we consider the ray to be the right of V2.
            bool isToRightOfV2 = ((microTriangle.N2.Cross(_r.O - microTriangle.V2).Dot(_r.D) > 0));

            // Our ray either passes through v0-v2 or v1-v2. If it passes through v0-v2,
            // then the current v1 is the only vertex that is not the same for the next triangle.
            // Since we always assume entering through v0-v1, v2 becomes v1 for the next triangle
            // and the next triangle's v2 is calculated from our heightmap data and the grid
            // location that we have not visited before.
            if (isToRightOfV2)
            {
                microTriangle.V0 = microTriangle.V2;
                barycentricPositions[0] = barycentricPositions[2];
            }
            else
            {
                microTriangle.V1 = microTriangle.V2;
                barycentricPositions[1] = barycentricPositions[2];
            }

            // +5 is the same as -1, but doesn't produce negative numbers when using modulo
            change = EGridChange(((uint32_t)change + (isToRightOfV2 ? 1 : 5)) % 6);
            switch (change)
            {
            case EGridChange::IPlus:
                if (++currentCell.i > numSubdivisions)
                    exitedGrid = true;
                barycentricPositions[2] = float2((currentCell.j + 1) * delta, (currentCell.k + 1) * delta);
                break;
            case EGridChange::JMin:
                if (--currentCell.j < 0)
                    exitedGrid = true;
                barycentricPositions[2] = float2(currentCell.j * delta, currentCell.k * delta);
                break;
            case EGridChange::KPlus:
                if (++currentCell.k > numSubdivisions)
                    exitedGrid = true;
                barycentricPositions[2] = float2(currentCell.j * delta, (currentCell.k + 1) * delta);
                break;
            case EGridChange::IMin:
                if (--currentCell.i < 0)
                    exitedGrid = true;
                barycentricPositions[2] = float2((currentCell.j + 1) * delta, currentCell.k * delta);
                break;
            case EGridChange::JPlus:
                if (++currentCell.j > numSubdivisions) 
                    exitedGrid = true;
                barycentricPositions[2] = float2((currentCell.j + 1) * delta, currentCell.k * delta);
                break;
            case EGridChange::KMin:
                if (--currentCell.k < 0) 
                    exitedGrid = true;
                barycentricPositions[2] = float2(currentCell.j * delta, (currentCell.k + 1) * delta);
                break;
            }
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

    float3 Triangle::GetCentroid() const
    {
        return (V0 + V1 + V2) / 3.0f;
    }

    AABB Triangle::GetBounds() const
    {
        AABB bounds;
        bounds.Min = float3::ComponentMin({ V0, V1, V2 });
        bounds.Max = float3::ComponentMax({ V0, V1, V2 });
        return bounds;
    }

    AABB Triangle::GetDisplacedBounds(float _maxHeight) const
    {
        std::array<float3, 3> vertices{ V0, V1, V2 };
        std::array<float3, 3> normals{ N0, N1, N2 };
        std::array<float3, 3> displacedVerticesMin;
        std::array<float3, 3> displacedVerticesMax;

        for (int i = 0; i < vertices.size(); i++)
        {
            displacedVerticesMin[i] = vertices[i] - (normals[i] * _maxHeight);
            displacedVerticesMax[i] = vertices[i] + (normals[i] * _maxHeight);
        }

        AABB bounds;
        bounds.Min = float3::ComponentMin({ displacedVerticesMin[0], displacedVerticesMin[1], displacedVerticesMin[2] });
        bounds.Max = float3::ComponentMax({ displacedVerticesMax[0], displacedVerticesMax[1], displacedVerticesMax[2] });
        return bounds;
    }
    float2 Triangle::GetUV(float3 _point, float3 _normal) const
    {
        // Calculated during intersection
        return float2{};
    }

    bool Cell::operator==(const Cell& other) const
    {
        return i == other.i && j == other.j && k == other.k;
    }
}
