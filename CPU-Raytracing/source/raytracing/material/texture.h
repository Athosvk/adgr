#pragma once
#include "./core/math/float2.h"
#include "./core/math/float4.h"

#include <string>

namespace CRT
{
	class Texture
	{
	public:
		Texture(const std::string& _filepath);

		float4 GetValue(float2 _uv);

	private:
		int m_Width;
		int m_Height;

		float* m_Buffer;
	};
}