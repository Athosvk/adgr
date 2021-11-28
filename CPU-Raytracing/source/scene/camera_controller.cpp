#include "camera_controller.h"

#include <./GLFW/glfw3.h>

#include "./raytracing/camera.h"

namespace CRT
{
	CameraController::CameraController(Camera& _camera) :
		m_Camera(_camera)
	{
	}

	void CameraController::ProcessInput(GLFWwindow* _window)
	{
		float3 position = m_Camera.GetPosition();
		if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS)
		{
			m_Camera.SetPosition(position + m_Speed * m_Camera.GetFront());
		}
		if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS)
		{
			m_Camera.SetPosition(position - m_Speed * m_Camera.GetRight());
		}
		if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS)
		{
			m_Camera.SetPosition(position - m_Speed * m_Camera.GetFront());
		}
		if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS)
		{
			m_Camera.SetPosition(position + m_Speed * m_Camera.GetRight());
		}
	}
}
