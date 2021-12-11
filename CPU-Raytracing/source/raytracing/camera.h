#pragma once
#include "core/math/float2.h"
#include "core/math/float3.h"
#include "ray.h"

#include <glm/mat4x4.hpp>

namespace CRT
{
	class Camera
	{
	public:
		Camera(float2 _viewportSize);

		void SetPosition(float3 _position);
		float3 GetPosition() const;
		Ray ConstructRay(float2 _uv) const;
		void SetDirection(float3 _direction);
		float3 GetFront() const;
		float3 GetUp() const;
		float3 GetRight() const;
		void SetFieldOfView(float angle);
		float GetFieldOfView() const;
		uint32_t GetAntiAliasing() const;
		void SetAntiAliasing(uint32_t aaFactor);
	private:
		glm::mat4 ConstructView() const;
		float3 Transform(float3 _toTranform, glm::mat4 _transform) const;

		float m_FocalLength = 1.0f;
		float2 m_ViewportSize;
		float3 m_Position = { 0.0f, 0.0f, 0.0f };
		float3 m_Front;
		float3 m_Up;
		float3 m_Right;
		glm::mat4 m_View;

		int m_AntiAliasing = 1;
	};
}
