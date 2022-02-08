#include "./raytracing/shapes/triangle.h"
#include "./raytracing/shapes/plane.h"
#include "./raytracing/shapes/bilinear_patch.h"

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
            _m.Barycentric = barycentric;
            return true;
        }

        return false;
    }

    bool Triangle::IntersectDisplacedNaive(Ray _r, Manifest& _m, const Texture* _heightmap, Cell& _cell) const
    {
        uint32_t numSubdivisions = 2;

        float delta = 1.0f / numSubdivisions;
        bool exitedGrid = false;

        bool intersected = false;
        for (int j = 0; j < numSubdivisions; j++)
        {
            for (int k = 0; k < numSubdivisions; k++)
            {
                for (int i = 0; i < numSubdivisions; i++)
                {
                    float2 uvV0, uvV1, uvV2;

                    // Upper triangle test
                    if ((i + j + k) % 2 == 0)
                    {
						// Upper left
						uvV0 = float2(j * delta, (k + 1) * delta);
						// Lower right
						uvV1 = float2((j + 1) * delta, k * delta);
                        // Upper right
                        uvV2 = float2((j + 1) * delta, (k + 1) * delta);
                    }
                    else
                    {
						// Lower right
						uvV0 = float2((j + 1) * delta, k * delta);
						// Upper left
						uvV1 = float2(j * delta, (k + 1) * delta);
                        // Lower left
                        uvV2 = float2(j * delta, k * delta);
                    }
                    if (uvV0.x + uvV0.y > 1) continue;
                    if (uvV1.x + uvV1.y > 1) continue;
                    if (uvV2.x + uvV2.y > 1) continue;
                    Triangle microTriangle;
                    Barycentric(microTriangle.V0, microTriangle.N0, microTriangle.u0, UVToBarycentric(uvV0));
                    Barycentric(microTriangle.V1, microTriangle.N1, microTriangle.u1, UVToBarycentric(uvV1));
                    Barycentric(microTriangle.V2, microTriangle.N2, microTriangle.u2, UVToBarycentric(uvV2));

                    microTriangle.V0 += _heightmap->GetValue(microTriangle.u0).x * microTriangle.N0;
                    microTriangle.V1 += _heightmap->GetValue(microTriangle.u1).x * microTriangle.N1;
                    microTriangle.V2 += _heightmap->GetValue(microTriangle.u2).x * microTriangle.N2;

                    Manifest manifest;
                    if (microTriangle.Intersect(_r, manifest) && manifest.T < _m.T)
                    {
                        _cell = Cell{ i, j, k };
                        _m = manifest;
                        return true;
                        intersected = true;
                    }
                }
            }
        }
        return intersected;
    }

    bool Triangle::IntersectTriangularSide(Ray _r, Triangle _tr, float _m, Cell& _cell, EGridChange& _startChange, EGridChange& _startChange2, float& _t, float _tesselation) const
    {
        // NOTE: This isn't always correct, sometimes we intersect no side patch of this cell, but still do not have
        // the same start as stop cell. Currently unknown as to why
        Manifest ma;
        if (_tr.Intersect(_r, ma))
        {
            _t = ma.T;
            float3 bary = ma.Barycentric;

			Triangle cellTriangle = GetCell(bary, _tesselation);

			float t = FLT_MAX, t00 = FLT_MAX;

			// Simulate a wedge, as we want to know if the ray intersected the cell planes in the upper and lower extents as it may
			// be near orthogonal to the cell, in which case it intersected the cell plane at some far far away point
			float prismWedgeExtents = 3.f;
            float3 inter1;
            float3 epsilon = float3(0.0001f);
			if (IntersectSidePatch(_r, cellTriangle.V0 - epsilon, cellTriangle.V1 + epsilon, cellTriangle.N0, cellTriangle.N1, prismWedgeExtents, t, inter1))
			{
				if (t < t00)
				{
					t00 = t;
					_startChange = EGridChange::JPlus;
					_startChange2 = EGridChange::IPlus;
				}
			}
			if (IntersectSidePatch(_r, cellTriangle.V1 - epsilon, cellTriangle.V2 + epsilon, cellTriangle.N1, cellTriangle.N2, prismWedgeExtents, t, inter1))
			{
				if (t < t00)
				{
					t00 = t;
					_startChange = EGridChange::KPlus;
					_startChange2 = EGridChange::JPlus;
				}
			}

			if (IntersectSidePatch(_r, cellTriangle.V2 - epsilon, cellTriangle.V0 + epsilon, cellTriangle.N2, cellTriangle.N0, prismWedgeExtents, t, inter1))
			{
				if (t < t00)
				{
					t00 = t;
					_startChange = EGridChange::IPlus;
					_startChange2 = EGridChange::KPlus;
				}
			}

			_cell = Cell::FromBarycentric(bary, _tesselation);

			// Upper triangles always move into a "negative" direction for i, j, k
			if (_cell.IsUpperTriangle(_tesselation))
			{
				_startChange = InvertGridchange(_startChange);
				_startChange2 = InvertGridchange(_startChange2);
			}
            return true;
		}
		return false;
	}

    Triangle Triangle::GetCell(float3 _bary, unsigned _tesselation) const
    {
        // _bary contains indices in order i, j, k
        Cell cell = Cell{ int32_t(_bary.x * _tesselation), int32_t(_bary.y * _tesselation),
            int32_t(_bary.z * _tesselation) };

        Triangle triangle;
        // if lower triangle
        // The "upperleft" vertex of the microtriangl2
        float2 uvV0 = float2(cell.j, cell.k) / _tesselation;
        float3 baryV0 = UVToBarycentric(uvV0);
        triangle.V0 = baryV0.x * V0 + baryV0.y * V1 + baryV0.z * V2;
        triangle.N0 = baryV0.x * N0 + baryV0.y * N1 + baryV0.z * N2;

        // The "lowerright" vertex of the microtriangle
        float2 uvV1 = float2(cell.j, cell.k + 1) / _tesselation;
        float3 baryV1 = UVToBarycentric(uvV1);
        triangle.V1 = baryV1.x * V1 + baryV1.y * V1 + baryV1.z * V2;
        triangle.N1 = baryV1.x * N0 + baryV1.y * N1 + baryV1.z * N2;

        // The "lowerleft" or "upperright" vertex of the microtriangle, depending on wehther
        // this is the lower or upper triangle
        float2 uvV2 = (cell.IsUpperTriangle(_tesselation) ? float2(cell.j + 1, cell.k + 1) :
                                                            float2(cell.j, cell.k)) / _tesselation;
        float3 baryV2 = UVToBarycentric(uvV2);
        triangle.V2 = baryV2.x * V0 + baryV2.y * V1 + baryV2.z * V2;
        triangle.N2 = baryV2.x * N0 + baryV2.y * N1 + baryV2.z * N2;
        return triangle;
    }

    bool Triangle::InitializeDisplaced(Ray _r, Cell& _start, Cell& _stop, EGridChange& _startChange, EGridChange& _startChange2, float& t, uint32_t _tesselation) const
    {
        float m = 1.0f;
        float t0 = FLT_MAX, t1 = FLT_MAX;

        Triangle tr0(V0 + N0 * m, V1 + N1 * m, V2 + N2 * m, u0, u1, u2, N0, N1, N2);
        EGridChange possibleStartChange = EGridChange::Uninit;
        EGridChange possibleStartChange2 = EGridChange::Uninit;
        bool enteredTop = false;
        if (IntersectTriangularSide(_r, tr0, m, _stop, possibleStartChange, possibleStartChange2, t1, _tesselation) && t1 < t0)
        {
            SwapIntersection(t0, _start, t1, _stop);
            _startChange = possibleStartChange;
            _startChange2 = possibleStartChange2;
        }

        Triangle tr1(V0 + N0 * -m, V1 + N1 * -m, V2 + N2 * -m, u0, u1, u2, -N0, -N1, -N2);
        if (IntersectTriangularSide(_r, tr1, m, _stop, possibleStartChange, possibleStartChange2, t1, _tesselation) && t1 < t0)
        {
            SwapIntersection(t0, _start, t1, _stop);
            _startChange = possibleStartChange;
            _startChange2 = possibleStartChange2;
        }

        float3 inter1;
        if (IntersectSidePatch(_r, V0, V1, N0, N1, m, t1, inter1))
        {
            float len = (V1 - V0).Magnitude();
            float3 nx = (V1 - V0).Normalize();
            float  nb = (inter1 - V0).Dot(nx) / len;

            _stop = Cell::FromBarycentric(float3(1.0f - nb, nb, 0.0f), _tesselation);
            if (t1 < t0)
            {
                SwapIntersection(t0, _start, t1, _stop);
                _startChange = EGridChange::KPlus;
                _startChange2 = EGridChange::KPlus;
            }
        }
        if ( IntersectSidePatch(_r, V1, V2, N1, N2, m, t1, inter1))
        {
            float len = (V2 - V1).Magnitude();
            float3 nx = (V2 - V1).Normalize();
            float  nb = (inter1 - V1).Dot(nx) / len;

            _stop = Cell::FromBarycentric(float3(0.0f, 1.0f - nb, nb), _tesselation);
            if (t1 < t0)
            {
                SwapIntersection(t0, _start, t1, _stop);
                _startChange = EGridChange::IPlus;
                _startChange2 = EGridChange::IPlus;
            }
        }
        if ( IntersectSidePatch(_r, V2, V0, N2, N0, m, t1, inter1))
        {
            float len = (V0 - V2).Magnitude();
            float3 nx = (V0 - V2).Normalize();
            float  nb = (inter1 - V2).Dot(nx) / len;

            _stop = Cell::FromBarycentric(float3(nb, 0.0f, 1.0f - nb), _tesselation);
            if (t1 < t0)
            {
                SwapIntersection(t0, _start, t1, _stop);
                _startChange = EGridChange::JPlus;
                _startChange2 = EGridChange::JPlus;
            }
        }

        if (t0 < FLT_MAX && t1 < FLT_MAX)
        {
            if (_start == _stop)
            {
                _startChange = EGridChange::None;
                possibleStartChange2 = EGridChange::None;
            }
            t = t0;
            return true;
        }
        return false;
    }

    void Triangle::SwapIntersection(float& _t0, Cell& _cell0, float& _t1, Cell& _cell1) const
    {
        if (_t0 < FLT_MAX)
        {
            std::swap(_t0, _t1);
            std::swap(_cell0.i, _cell1.i);
            std::swap(_cell0.j, _cell1.j);
            std::swap(_cell0.k, _cell1.k);
        }
        else
        {
            _t0 = _t1;
            _cell0 = _cell1;
        }
    }

    bool Triangle::IntersectSidePatch(Ray _r, float3 _p0, float3 _p1, float3 _n0, float3 _n1, float _m, float& _t, float3& _intersectionPoint) const
    {
        BilinearPatch patch(_p0 - _n0 * _m, _p1 - _n1 * _m,
            _p0 + _n0 * _m, _p1 + _n1 * _m);
        Manifest manifest;
        if (patch.Intersect(_r, manifest))
        {
            _t = manifest.T;
            _intersectionPoint = manifest.IntersectionPoint;
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

    bool Triangle::IntersectDisplaced(Ray _r, Manifest& _m, const Texture* heightMap) const
    {
        Cell startCell;
        Cell stopCell;
        EGridChange startChange;
        EGridChange startChange2;
        
        float3 triangleCenter;
        float3 n;
        float2 uv;
        Barycentric(triangleCenter, n, uv, UVToBarycentric(float2{ 0.5f, 0.5f }));

        float distance = (_r.O - triangleCenter).Magnitude();
        float t;
        uint32_t tesselation = std::min(std::max(2.f, 300 * (1 / distance)), 512.f);
        if (!InitializeDisplaced(_r, startCell, stopCell, startChange, startChange2, t, tesselation))
        {
            return false;
        }
        bool intersected = WalkIntersection(_r, _m, heightMap, startCell, stopCell, startChange, tesselation);
        if (startChange != startChange2)
        {
           intersected |= WalkIntersection(_r, _m, heightMap, startCell, stopCell, startChange2, tesselation);
        }
        //Cell realCell;
        //Manifest manifest;
   //     if (!intersected && startChange != EGridChange::None)
   //     {
   //         if (false && IntersectDisplacedNaive(_r, manifest, heightMap, realCell))
   //         {
   //             Cell newStart;
   //             Cell newStop;
   //             EGridChange change;
   //             EGridChange change2;
   //             float t;
   //             //__debugbreak();
   //             if (InitializeDisplaced(_r, newStart, newStop, change, change2, t, tesselation))
   //             {
   //             }
			//	intersected = WalkIntersection(_r, _m, heightMap, startCell, stopCell, startChange, tesselation);
			//	if (startChange != startChange2)
			//	{
			//		intersected |= WalkIntersection(_r, _m, heightMap, startCell, stopCell, startChange2, tesselation);
			//	}
			//}
   //     }
        return intersected;
    }

    bool Triangle::WalkIntersection(Ray _r, Manifest& _m, const Texture* _heightmap, Cell _startCell, Cell _stopCell, EGridChange _startChange, float _tesselation) const
    {
        Cell currentCell = _startCell;
        EGridChange change = _startChange;
         //_m.ShadingNormal = float3(stopCell.i / 2.0f, stopCell.j / 2.0f, stopCell.k / 2.0f);
         //if (startCell == stopCell)
         //    _m.ShadingNormal = float3(1.0f, 1.0f, 1.0f);
         ////_m.ShadingNormal = float3(stopCell.i / 2.0f, stopCell.j / 2.0f, stopCell.k / 2.0f);
         ////if (change != EGridChange::None)
         ////    _m.ShadingNormal = float3(change == EGridChange::IMin ? 0.5f : change == EGridChange::IPlus ? 1.0f : 0.0f, change == EGridChange::JMin ? 0.5f : change == EGridChange::JPlus ? 1.0f : 0.0f, change == EGridChange::KMin ? 0.5f : change == EGridChange::KPlus ? 1.0f : 0.0f);
         ////else
         ////    _m.ShadingNormal = float3::One();
         //return true;
        //{
        //   return false;
        //}

        std::array<float2, 3> uvPositions;

        switch (change)
        {
        case CRT::EGridChange::IPlus:
            uvPositions[0] = float2(currentCell.j + 1, currentCell.k) / _tesselation;
            uvPositions[1] = float2(currentCell.j, currentCell.k + 1) / _tesselation;
            uvPositions[2] = float2(currentCell.j, currentCell.k) / _tesselation;
            break;
        case CRT::EGridChange::JMin:
            uvPositions[0] = float2(currentCell.j + 1, currentCell.k) / _tesselation;
            uvPositions[1] = float2(currentCell.j + 1, currentCell.k + 1) / _tesselation;
            uvPositions[2] = float2(currentCell.j, currentCell.k + 1) / _tesselation;
            break;
        case CRT::EGridChange::KPlus:
            uvPositions[0] = float2(currentCell.j, currentCell.k) / _tesselation;
            uvPositions[1] = float2(currentCell.j + 1, currentCell.k) / _tesselation;
            uvPositions[2] = float2(currentCell.j, currentCell.k + 1) / _tesselation;
            break;
        case CRT::EGridChange::IMin:
            uvPositions[0] = float2(currentCell.j, currentCell.k + 1) / _tesselation;
            uvPositions[1] = float2(currentCell.j + 1, currentCell.k) / _tesselation;
            uvPositions[2] = float2(currentCell.j + 1, currentCell.k + 1) / _tesselation;
            break;
        case CRT::EGridChange::JPlus:
            uvPositions[0] = float2(currentCell.j, currentCell.k) / _tesselation;
            uvPositions[1] = float2(currentCell.j, currentCell.k + 1) / _tesselation;
            uvPositions[2] = float2(currentCell.j + 1, currentCell.k) / _tesselation;
            break;
        case CRT::EGridChange::KMin:
            uvPositions[0] = float2(currentCell.j + 1, currentCell.k + 1) / _tesselation;
            uvPositions[1] = float2(currentCell.j, currentCell.k + 1) / _tesselation;
            uvPositions[2] = float2(currentCell.j + 1, currentCell.k) / _tesselation;
            break;
        default:
            uvPositions[0] = float2(currentCell.j + 1, currentCell.k) / _tesselation;
            uvPositions[1] = float2(currentCell.j, currentCell.k + 1) / _tesselation;
            if (currentCell.IsUpperTriangle(_tesselation))
            {
                uvPositions[2] = float2(currentCell.j + 1, currentCell.k + 1) / _tesselation;
            }
            else
            {
                uvPositions[2] = float2(currentCell.j, currentCell.k) / _tesselation;
            }
            break;
        }

        Triangle microTriangle;
        float delta = 1.0f / _tesselation;
        bool exitedGrid = false;

        Barycentric(microTriangle.V0, microTriangle.N0, microTriangle.u0, UVToBarycentric(uvPositions[0]));
        Barycentric(microTriangle.V1, microTriangle.N1, microTriangle.u1, UVToBarycentric(uvPositions[1]));

        microTriangle.V0 += _heightmap->GetValue(microTriangle.u0).x * microTriangle.N0;
        microTriangle.V1 += _heightmap->GetValue(microTriangle.u1).x * microTriangle.N1;
        int iteration = 0;

        size_t numTriangles = 0;
        while (!exitedGrid)
        {
            numTriangles++;
            Barycentric(microTriangle.V2, microTriangle.N2, microTriangle.u2, UVToBarycentric(uvPositions[2]));
            microTriangle.V2 += _heightmap->GetValue(microTriangle.u2).x * microTriangle.N2;

            Manifest nearest;
            if (microTriangle.Intersect(_r, nearest))
            {
                _m = nearest;
                return true;
            }
            if (currentCell == _stopCell)
            {
                break;
            }

            // First get the vector perpendicular to the V2 normal and the direction towards the ray origin.
            // If the incoming ray projects to the right of V2, which is where this cross product should be aimed to,
            // then we consider the ray to be the right of V2.
            bool isToRightOfV2 = microTriangle.N2.Cross(_r.O - microTriangle.V2).Dot(_r.D) > 0;

            // Our ray either passes through v0-v2 or v1-v2. If it passes through v0-v2,
            // then the current v1 is the only vertex that is not the same for the next triangle.
            // Since we always assume entering through v0-v1, v2 becomes v1 for the next triangle
            // and the next triangle's v2 is calculated from our heightmap data and the grid
            // location that we have not visited before.
            if (isToRightOfV2)
            {
                microTriangle.V0 = microTriangle.V2;
                microTriangle.N0 = microTriangle.N2;
                microTriangle.u0 = microTriangle.u2;
                uvPositions[0] = uvPositions[2];
            }
            else
            {
                microTriangle.V1 = microTriangle.V2;
                microTriangle.N1 = microTriangle.N2;
                microTriangle.u1 = microTriangle.u2;
                uvPositions[1] = uvPositions[2];
            }

            // +5 is the same as -1, but doesn't produce negative numbers when using modulo
            change = EGridChange(((uint32_t)change + (isToRightOfV2 ? 1 : 5)) % 6);
            switch (change)
            {
            case EGridChange::IMin:
                if (--currentCell.i < 0)
                    exitedGrid = true;
                uvPositions[2] = float2((currentCell.j + 1) * delta, (currentCell.k + 1) * delta);
                break;
            case EGridChange::IPlus:
                if (++currentCell.i >= _tesselation)
                    exitedGrid = true;
                uvPositions[2] = float2(currentCell.j * delta, currentCell.k * delta);
                break;
            case EGridChange::JMin:
                if (--currentCell.j < 0)
                    exitedGrid = true;
                uvPositions[2] = float2(currentCell.j * delta, (currentCell.k + 1) * delta);
                break;
            case EGridChange::JPlus:
                if (++currentCell.j >= _tesselation)
                    exitedGrid = true;
                uvPositions[2] = float2((currentCell.j + 1) * delta, currentCell.k * delta);
                break;
            case EGridChange::KMin:
                if (--currentCell.k < 0)
                    exitedGrid = true;
                uvPositions[2] = float2((currentCell.j + 1) * delta, currentCell.k * delta);
                break;
            case EGridChange::KPlus:
                if (++currentCell.k >= _tesselation)
                    exitedGrid = true;
                uvPositions[2] = float2(currentCell.j * delta, (currentCell.k + 1) * delta);
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
        bounds.Min = float3::ComponentMin({ displacedVerticesMin[0], displacedVerticesMin[1], displacedVerticesMin[2], displacedVerticesMax[0], displacedVerticesMax[1], displacedVerticesMax[2] });
        bounds.Max = float3::ComponentMax({ displacedVerticesMax[0], displacedVerticesMax[1], displacedVerticesMax[2], displacedVerticesMin[0], displacedVerticesMin[1], displacedVerticesMin[2] });
        return bounds;
    }

    float2 Triangle::GetUV(float3 _point, float3 _normal) const
    {
        // Calculated during intersection
        return float2{};
    }

    float3 Triangle::UVToBarycentric(float2 _uv) const
    {
        return float3{ 1.0f - _uv.x - _uv.y, _uv.x, _uv.y };
    }

    Cell Cell::FromBarycentric(float3 _baryCentric, float _tesselation)
    {
        Cell cell { int32_t(_baryCentric.x * _tesselation), int32_t(_baryCentric.y * _tesselation),
            int32_t(_baryCentric.z * _tesselation) };
        // Edge case, literally. We are the edge of a cell but shoot slightly over it, out of the triangle
        // Fix by re-adjusting the highest coordinate slightly back into the cell that it belongs to
        if (cell.i + cell.j + cell.k >= _tesselation)
        {
            if (_baryCentric.x > _baryCentric.y && _baryCentric.x > _baryCentric.z)
            {
                cell.i--;
            }
            else if (_baryCentric.y > _baryCentric.z)
            {
                cell.j--;
            }
            else
            {
                cell.k--;
            }
        }
        return cell;
    }

    bool Cell::IsUpperTriangle(uint32_t _tesselation) const
    {
        // Indicies for upper triangles sum to N - 2
        return (i + j + k) == _tesselation - 2;
    }

    bool Cell::operator==(const Cell& other) const
    {
        return i == other.i && j == other.j && k == other.k;
    }

    EGridChange InvertGridchange(EGridChange _change)
    {
        return EGridChange(((uint8_t)_change + ((uint8_t)EGridChange::IMin - (uint8_t)EGridChange::IPlus)) % 6);
    }
}
