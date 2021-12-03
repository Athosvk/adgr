#pragma once
#include "./core/math/float3.h"
#include "./raytracing/material/texture.h"

namespace CRT
{
	class Texture;

	class Material
	{
	public:
		Material(float3 _color, float _specularity, Texture* _texture)
			: Color(_color)
			, Specularity(_specularity)
			, Texture(_texture)
		{ }

		float3 Color;
		float  Specularity;
		Texture* Texture;
	};
}