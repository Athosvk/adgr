#pragma once
struct GLFWwindow;

#include "./core/math/float2.h"
#include "./core/math/trigonometry.h"

namespace CRT
{
	class Camera;

	class CameraController
	{
	public:
		CameraController(Camera& _camera);

		void ProcessInput(GLFWwindow* _window, float _deltaTime);
		void Reset();
	private:
		void RotateCamera();
		float2 GetMousePosition(GLFWwindow* _window) const;

		Camera& m_Camera;
		float m_MovementSpeed = 10.0f;
		bool m_Rotating = false;
		float2 m_MousePosition = { 0.0f, 0.0f };
		float m_YawDelta = 0.f;
		float m_PitchDelta = 0.f;
		float m_Yaw = -90.;
		float m_Pitch = -0.f;
	};
}
