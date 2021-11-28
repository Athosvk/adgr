#include "./core/graphics/screen/surface.h"

#include <stdlib.h>

namespace CRT
{
	Surface::Surface(const uint32_t _width, const uint32_t _height)
		: m_Width(_width)
		, m_Height(_height)
	{
		// Allign buffer to 64
		m_Buffer = (Pixel*)_aligned_malloc(m_Width * m_Height * sizeof(Pixel), 64);
	}

	Surface::Surface(const uint32_t _width, const uint32_t _height, Pixel* _buffer)
		: m_Width(_width)
		, m_Height(_height)
		, m_Buffer(_buffer)
	{ }

	Surface::~Surface()
	{
		_aligned_free(m_Buffer);
	}

	void Surface::Set(const uint32_t _x, const uint32_t _y, const Pixel _p)
	{
		m_Buffer[_x + _y * m_Width] = _p;
	}
}