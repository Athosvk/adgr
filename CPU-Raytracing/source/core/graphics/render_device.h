#pragma once
#include "./core/window/window.h"
#include "./core/graphics/screen/surface.h"

namespace CRT
{
	class Shader;
	class RenderDevice
	{
	public:
		RenderDevice(Window* _window);
		~RenderDevice();

		void CopyFrom(Surface* _surface);
		void Present();

	private:

		uint32_t m_RenderTargetID;
		Shader*  m_Shader = nullptr;
	};
}