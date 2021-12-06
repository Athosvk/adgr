#pragma once
#include "./material.h"

namespace CRT
{
	class Basic : public Material
	{
		Basic(float3 _color, Texture* _texture, float _specularity) :
			Material(_color, _specularity, _texture)
		{
		}
	};
}
