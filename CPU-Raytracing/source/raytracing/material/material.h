#pragma once
#include "./core/math/float3.h"
#include "./raytracing/material/texture.h"

namespace CRT
{
	class Texture;

	enum class Type
	{
		Basic,
		Dielectric
	};

	class Material
	{
	public:
		Material(float3 _color, float _specularity, Texture* _texture, Texture* _heightmap = nullptr)
			: Color(_color)
			, Specularity(_specularity)
			, Texture(_texture)
			, HeightMap(_heightmap)
		{ }

		Texture* HeightMap;
		Texture* Texture;

		float3 Color;
		float Specularity;
		float RefractionIndex;
		Type type = Type::Basic;
	};
}
