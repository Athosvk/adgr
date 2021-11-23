#pragma once
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
		Material* M;
	};
}