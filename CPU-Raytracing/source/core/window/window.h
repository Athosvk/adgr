#pragma once
#include <./GL/glew.h>
#include <./GLFW/glfw3.h>

#include <string>
#include <stdint.h>

namespace CRT
{
	class Window
	{
	public:
		Window(const std::string& _title, const uint32_t _width, const uint32_t _height);
		virtual ~Window();

		void PollEvents();
		bool ShouldClose();

		inline uint32_t GetWidth() const { return m_ClientWidth; }
		inline uint32_t GetHeight() const { return m_ClientHeight; }

		inline GLFWwindow* GetWindow() const { return m_Window; }

	protected:

		std::string m_Title;

		uint32_t	m_ClientWidth;
		uint32_t	m_ClientHeight;

		GLFWwindow* m_Window;
	};
}