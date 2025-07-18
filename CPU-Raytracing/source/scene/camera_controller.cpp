#include "camera_controller.h"

#include <./GLFW/glfw3.h>

#include "./raytracing/camera.h"
#include "./core/math/trigonometry.h"

#include <algorithm>

namespace CRT
{
	CameraController::CameraController(Camera& _camera) :
		m_Camera(_camera)
	{
	}

	void CameraController::ProcessInput(GLFWwindow* _window, float _deltaTime)
	{
		float3 position = m_Camera.GetPosition();
		// Basically for debug mode, prevents movement being incredibly jumpy
		float cappedDeltaTime = std::min(_deltaTime, 0.5f);
		if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS)
		{
			m_Camera.SetPosition(position + m_MovementSpeed * m_Camera.GetFront() * cappedDeltaTime);
		}
		if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS)
		{
			m_Camera.SetPosition(position - m_MovementSpeed * m_Camera.GetRight() * cappedDeltaTime);
		}
		if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS)
		{
			m_Camera.SetPosition(position - m_MovementSpeed * m_Camera.GetFront() * cappedDeltaTime);
		}
		if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS)
		{
			m_Camera.SetPosition(position + m_MovementSpeed * m_Camera.GetRight() * cappedDeltaTime);
		}

		if (glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && !m_Rotating)
		{
			m_Rotating = true;
			glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			m_MousePosition = GetMousePosition(_window);
		}
		else if ((glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) && m_Rotating)
		{
			m_Rotating = false;
			glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		if (m_Rotating)
		{
			float2 new_mouse = GetMousePosition(_window);
			float2 delta_mouse_position = new_mouse - m_MousePosition;
			m_YawDelta = delta_mouse_position.x * cappedDeltaTime ;
			m_PitchDelta = delta_mouse_position.y * cappedDeltaTime;
			m_MousePosition = new_mouse;
		}
		m_Pitch += m_PitchDelta;
		m_Yaw += m_YawDelta;

		// Smoothing of camera rotation
		m_YawDelta *= 0.5f;
		m_PitchDelta *= 0.5f;

		RotateCamera();
	}

	void CameraController::Reset()
	{
		m_Pitch = 0.0f;
		m_Yaw = -90.f;
	}

	void CameraController::RotateCamera()
	{
		m_Pitch = glm::clamp(m_Pitch, -89.f, 89.f);
		glm::vec3 direction;
		float cosPitch = std::cosf(ToRadians(m_Pitch));
		direction.x = std::cosf(ToRadians(m_Yaw)) * cosPitch;
		direction.y = std::sinf(ToRadians(m_Pitch));
		direction.z = std::sinf(ToRadians(m_Yaw)) * cosPitch;
		direction = glm::normalize(direction);
		m_Camera.SetDirection({ direction.x, direction.y, direction.z });
	}

	float2 CameraController::GetMousePosition(GLFWwindow* _window) const
	{
		double mouse_position[2];
		glfwGetCursorPos(_window, mouse_position, &mouse_position[1]);
		return float2 { (float)mouse_position[0], (float)mouse_position[1] };
	}
}
