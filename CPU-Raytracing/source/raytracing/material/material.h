#pragma once
#include "./core/math/float3.h"

namespace CRT
{
	class Material
	{
	public:
		Material(float3 _color, float _specularity)
			: Color(_color)
			, Specularity(_specularity)
		{ }

		float3 Color;
		float  Specularity;
	};
}