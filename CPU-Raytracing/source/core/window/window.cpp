#include "./core/window/window.h"

namespace CRT
{
	// Static
	static Window* s_Window = nullptr;

	Window::Window(const std::string& _title, const uint32_t _width, const uint32_t _height)
		: m_ClientWidth(_width)
		, m_ClientHeight(_height)
	{
		glfwInit();
		glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

		// Open a window and create its OpenGL context
		m_Window = glfwCreateWindow(_width, _height, _title.c_str(), NULL, NULL);
		if (m_Window == NULL) {
			fprintf(stderr, "Failed to open a window.\n");
			glfwTerminate();
		}
		glfwMakeContextCurrent(m_Window);
	}

	Window::~Window()
	{
		glfwDestroyWindow(m_Window);
	}

	void Window::PollEvents()
	{
		glfwSwapBuffers(m_Window);
		glfwPollEvents();
	}

	bool Window::ShouldClose()
	{
		return glfwWindowShouldClose(m_Window);
	}
}