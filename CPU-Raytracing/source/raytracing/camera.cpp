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
		m_ViewportSize(_viewportSize),
		m_Front(float3::Forward()),
		m_Up(float3::Up()),
		m_Right(float3::Right()),
		m_View(ConstructView())
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
		float3 cameraPlane = m_FocalLength * focalDirection;
		float3 p0 = cameraPlane + float3(-1, 1, 0);
		float3 p1 = cameraPlane + float3(1, 1, 0);
		float3 p2 = cameraPlane + float3(-1, -1, 0);

		float3 uv = p0 + (_uv.x * (p1 - p0)) + (_uv.y * (p2 - p0));
		float aspect_ratio = m_ViewportSize.x / (float)m_ViewportSize.y;
		uv.x *= aspect_ratio;
		float3 d = (Transform((uv), m_View)).Normalize();
		return Ray(m_Position, d);
	}

	void Camera::SetDirection(float3 _direction)
	{
		m_Front = _direction;
		m_Right = m_Front.Cross(float3::Up()).Normalize();
		m_Up = m_Right.Cross(m_Front).Normalize();
		m_View = ConstructView();
	}

	float3 Camera::GetFront() const
	{
		return m_Front;
	}

	float3 Camera::GetUp() const
	{
		return m_Up;
	}

	float3 Camera::GetRight() const
	{
		return m_Right;
	}

	float Camera::GetFocalLength() const
	{
		return m_FocalLength;
	}

	void Camera::SetFocalLength(float focalLength)
	{
		m_FocalLength = focalLength;
	}

	glm::mat4 Camera::ConstructView() const
	{
		glm::vec3 front = ToGlm(m_Front);
		// Only interested in direction, position will be used as ray origin
		return glm::inverse(glm::lookAt(ToGlm(float3::Zero()), front, ToGlm(float3::Up())));
	}

	float3 Camera::Transform(float3 _toTransform, glm::mat4 _transform) const
	{
		glm::vec4 toTransform = glm::vec4(ToGlm(_toTransform), 0.0f);
		glm::vec4 transformed = _transform * toTransform;
		return float3 { transformed.x, transformed.y, transformed.z };
	}
}
