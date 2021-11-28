#include "camera.h"

CRT::Camera::Camera(CRT::float2 _viewportSize) :
	m_ViewportSize(_viewportSize)
{
}

void CRT::Camera::SetPosition(CRT::float3 position)
{
	m_Position = position;
}

CRT::float3 CRT::Camera::GetPosition() const
{
	return m_Position;
}

CRT::Ray CRT::Camera::ConstructRay(float2 uv_) const
{
	float3 focalDirection(0.0, 0.0, -1.0);
	float3 cameraPlane = (m_FocalLength * focalDirection) + m_Position;
	float3 p0 = cameraPlane + float3(-1,  1, 0);
	float3 p1 = cameraPlane + float3( 1,  1, 0);
	float3 p2 = cameraPlane + float3(-1, -1, 0);

	float3 uv = p0 + (uv_.x * (p1 - p0)) + (uv_.y * (p2 - p0));
	float aspect_ratio = m_ViewportSize.x / (float)m_ViewportSize.y;
	uv.x *= aspect_ratio;
	float3 d = (uv - m_Position).Normalize();
	return Ray(m_Position, d);
}
