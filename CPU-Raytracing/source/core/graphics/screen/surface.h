#pragma once
#include "./core/graphics/screen/pixel.h"

namespace CRT
{
	class Surface
	{
	public:
		Surface(const uint32_t _width, const uint32_t _height);
		Surface(const uint32_t _width, const uint32_t _height, Pixel* _buffer);
		~Surface();
		
		void Set(const uint32_t _x, const uint32_t _y, const Pixel _p);

		// Getters
		inline uint32_t GetWidth()  const { return m_Width; }
		inline uint32_t GetHeight() const { return m_Height; }

		inline Pixel* GetBuffer() const { return m_Buffer; }

	private:
		uint32_t m_Width;
		uint32_t m_Height;
		
		Pixel* m_Buffer = nullptr;
	};
}