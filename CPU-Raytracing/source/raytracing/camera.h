#pragma once
#include "core/math/float2.h"
#include "core/math/float3.h"
#include "ray.h"

namespace CRT
{
	class Camera
	{
	public:
		Camera(float2 _viewportSize);

		void SetPosition(float3 position);
		float3 GetPosition() const;
		Ray ConstructRay(float2 uv) const;
	private:
		double m_FocalLength = 1.0f;
		float2 m_ViewportSize;
		float3 m_Position = { 0.0, 0.0, 0.0 };
	};
}
