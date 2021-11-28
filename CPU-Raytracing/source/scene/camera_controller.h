#pragma once
struct GLFWwindow;

namespace CRT
{
	class Camera;

	class CameraController
	{
	public:
		CameraController(Camera& _camera);

		void ProcessInput(GLFWwindow* _window);
	private:
		Camera& m_Camera;
		float m_Speed = 0.1f;
	};
}
