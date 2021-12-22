#pragma once
#include "./core/math/float3.h"
#include <glm/mat4x4.hpp>

#include <limits.h>
#include <intrin.h>
#include <xmmintrin.h>

//#define USE_AVX

#if defined(USE_AVX)
#define JOB_INC 8
#else
#define JOB_INC 1
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
}
