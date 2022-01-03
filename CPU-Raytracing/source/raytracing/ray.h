#pragma once
#include "./core/math/float3.h"
#include <glm/mat4x4.hpp>

#include <limits.h>
#include <intrin.h>
#include <xmmintrin.h>

// #define USE_RAYPACKET
//#define USE_AVX

#define RAYPACKET_WIDTH 16
#define RAYPACKET_HEIGHT 16

#define RAYPACKET_TOP_LEFT 0
#define RAYPACKET_TOP_RIGHT 85
#define RAYPACKET_BOTTOM_LEFT 171
#define RAYPACKET_BOTTOM_RIGHT 255

#if defined(USE_AVX)
#define JOB_INC 8
#else
#if defined(USE_RAYPACKET)
#define JOB_INC RAYPACKET_WIDTH*RAYPACKET_HEIGHT
#else
#define JOB_INC 1
#endif
#endif

namespace CRT
{


	class Ray
	{
	public:
		Ray(float3 _o, float3 _d)
			: O(_o)
			, D(_d)
		{ }

		float3 Sample(float t) const;
		Ray Reflect(float3 _n);

		Ray Transform(const glm::mat4x4& _matrix) const;

		float3 O;
		float3 D;
	};

	struct TraversalResultPacket
	{
	public:
		TraversalResultPacket(const uint32_t _c)
			: T(new float[_c])
			, ID(new float[_c])
		{
			for (int i = 0; i < _c; i++)
				T[i] = FLT_MAX;
		}
		~TraversalResultPacket()
		{
			delete[] T;
			delete[] ID;
		}

		float* T;
		float* ID;
	};

	class RayPacket
	{
	public:
		RayPacket(float3 _o, float3* _d, const uint32_t _w, const uint32_t _h)
			: Width(_w)
			, Height(_h)
			, O(_o)
			, D(new float3[_w * _h])
		{
			for (int i = 0; i < Width * Height; i++)
			{
				D[i] = _d[i];
			}

			// Calculate frustum
			float3 corners[] = { O + _d[RAYPACKET_TOP_RIGHT], O + _d[RAYPACKET_TOP_LEFT], O + _d[RAYPACKET_BOTTOM_LEFT], O + _d[RAYPACKET_BOTTOM_RIGHT], };
			CalculateFrustum(corners);
		}
		~RayPacket()
		{
			delete[] D;
		}

		float3  O;
		float3* D;

		float3 P[4];
		float DD[4];

		uint32_t Width;
		uint32_t Height;

	private:
		void CalculateFrustum(float3* _corners);
	};

	class TraversalResult__m256
	{
	public:
		TraversalResult__m256()
			: T(_mm256_set1_ps(FLT_MAX))
		{ }

		union
		{
			struct { __m256 T; __m256 ID; };
			struct { float t[8]; float i[8]; };
		};
	};

	class OctRay
	{
	public:
		OctRay(float3 _o, float3* _d)
			: Ox(_mm256_setr_ps(_o.x, _o.x, _o.x, _o.x, _o.x, _o.x, _o.x, _o.x))
			, Oy(_mm256_setr_ps(_o.y, _o.y, _o.y, _o.y, _o.y, _o.y, _o.y, _o.y))
			, Oz(_mm256_setr_ps(_o.z, _o.z, _o.z, _o.z, _o.z, _o.z, _o.z, _o.z))
			, Dx(_mm256_setr_ps(_d[0].x, _d[1].x, _d[2].x, _d[3].x, _d[4].x, _d[5].x, _d[6].x, _d[7].x))
			, Dy(_mm256_setr_ps(_d[0].y, _d[1].y, _d[2].y, _d[3].y, _d[4].y, _d[5].y, _d[6].y, _d[7].y))
			, Dz(_mm256_setr_ps(_d[0].z, _d[1].z, _d[2].z, _d[3].z, _d[4].z, _d[5].z, _d[6].z, _d[7].z))
			, rDx(_mm256_rcp_ps(_mm256_setr_ps(_d[0].x, _d[1].x, _d[2].x, _d[3].x, _d[4].x, _d[5].x, _d[6].x, _d[7].x)))
			, rDy(_mm256_rcp_ps(_mm256_setr_ps(_d[0].y, _d[1].y, _d[2].y, _d[3].y, _d[4].y, _d[5].y, _d[6].y, _d[7].y)))
			, rDz(_mm256_rcp_ps(_mm256_setr_ps(_d[0].z, _d[1].z, _d[2].z, _d[3].z, _d[4].z, _d[5].z, _d[6].z, _d[7].z)))
		{ }

		void Sample(float t, __m256& _x, __m256& _y, __m256& _z) const;
		void Sample(__m256 t, __m256& _x, __m256& _y, __m256& _z) const;

		__m256 Ox;
		__m256 Oy;
		__m256 Oz;

		__m256 Dx;
		__m256 Dy;
		__m256 Dz;
		__m256 rDx;
		__m256 rDy;
		__m256 rDz;
	};

	inline uint64_t xy_to_morton(uint32_t x, uint32_t y)
	{
		return _pdep_u32(x, 0x55555555) | _pdep_u32(y, 0xaaaaaaaa);
	}

	inline void morton_to_xy(uint64_t m, uint32_t* x, uint32_t* y)
	{
		*x = _pext_u64(m, 0x5555555555555555);
		*y = _pext_u64(m, 0xaaaaaaaaaaaaaaaa);
	}
}
