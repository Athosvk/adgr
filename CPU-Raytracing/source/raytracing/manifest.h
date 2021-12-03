#pragma once
#include "./core/math/float2.h"
#include "./core/math/float3.h"
#include "./raytracing/material/material.h"

namespace CRT
{
	class Manifest
	{
	public:
		Manifest() = default;

		float     T = FLT_MAX;
		float3	  N;
		float2    UV;
		Material* M;
		float3 IntersectionPoint;
	};
}