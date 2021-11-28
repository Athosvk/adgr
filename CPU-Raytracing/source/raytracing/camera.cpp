#include "camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace CRT
{
	glm::vec3 ToGlm(float3 _value)
	{
		return glm::vec3(_value.x, _value.y, _value.z);
	}
	Camera::Camera(float2 _viewportSize) :
		m_ViewportSize(_viewportSize)
	{
	}

	void Camera::SetPosition(float3 position)
	{
		m_Position = position;
	}

	float3 Camera::GetPosition() const
	{
		return m_Position;
	}

	Ray Camera::ConstructRay(float2 _uv) const
	{
		float3 focalDirection(0.0, 0.0, -1.0);
		float3 cameraPlane = (m_FocalLength * focalDirection) + m_Position;
		float3 p0 = cameraPlane + float3(-1, 1, 0);
		float3 p1 = cameraPlane + float3(1, 1, 0);
		float3 p2 = cameraPlane + float3(-1, -1, 0);

		float3 uv = p0 + (_uv.x * (p1 - p0)) + (_uv.y * (p2 - p0));
		float aspect_ratio = m_ViewportSize.x / (float)m_ViewportSize.y;
		uv.x *= aspect_ratio;
		float3 d = Transform((uv - m_Position).Normalize(), ConstructView());
		return Ray(m_Position, d);
	}

	void Camera::SetDirection(float3 _direction)
	{
		m_Front = _direction;
	}

	float3 Camera::GetFront() const
	{
		return m_Front;
	}

	float3 Camera::GetUp() const
	{
		return float3::Up();
	}

	float3 Camera::GetRight() const
	{
		return m_Front.Cross(float3::Up()).Normalize();
	}

	glm::mat4 Camera::ConstructView() const
	{
		glm::vec3 position = ToGlm(m_Position);
		glm::vec3 front = ToGlm(m_Front);
		return glm::lookAt(position, position + front, ToGlm(GetUp()));
	}

	float3 Camera::Transform(float3 _toTransform, glm::mat4 _transform) const
	{
		glm::vec4 toTransform = glm::vec4(ToGlm(_toTransform), 0.0f);
		glm::vec4 transformed = _transform * toTransform;
		return float3 { transformed.x, transformed.y, transformed.z };
	}
}
