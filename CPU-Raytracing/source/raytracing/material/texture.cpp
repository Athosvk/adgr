#include "./raytracing/material/texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <./stb_image.h>
#include <./core/math/float3.h>

namespace CRT
{
	Texture::Texture(const std::string& _filepath)
	{
		// Load image Data
		int channels;
		stbi_set_flip_vertically_on_load(1);
		float* image = stbi_loadf(_filepath.c_str(), &m_Width, &m_Height, &channels, STBI_rgb_alpha);

		// Allign buffer to 64
		m_Buffer = (float*)_aligned_malloc(m_Width * m_Height * sizeof(float) * 4, 64);

		for (uint32_t i = 0; i < m_Width * m_Height; i++)
		{
			m_Buffer[i * 4 + 0] = image[i * 4 + 0]; // May break if channels is only 3, but is it?
			m_Buffer[i * 4 + 1] = image[i * 4 + 1];
			m_Buffer[i * 4 + 2] = image[i * 4 + 2];

			m_Buffer[i * 4 + 3] = channels == 4 ? image[i * 4 + 3] : 1.0f;
		}
	}

	float4 Texture::GetValue(float2 _uv) const
	{
		_uv = float3::ComponentMax({ float3(_uv), float3::Zero() });
		_uv = float3::ComponentMin({ float3(_uv), float3::One() });
		int x = int(float(m_Width - 1) * _uv.x);
		int y = int(float(m_Height - 1) * _uv.y);

		int id = (y * m_Width + x);
		return float4(m_Buffer[id * 4 + 0], m_Buffer[id * 4 + 1], m_Buffer[id * 4 + 2], m_Buffer[id * 4 + 3]);
	}
}