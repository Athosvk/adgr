#pragma once
#include "./raytracing/ray.h"
#include "./raytracing/shapes/shape.h"
#include "./raytracing/shapes/sphere.h"
#include "./raytracing/shapes/plane.h"
#include "./raytracing/shapes/torus.h"
#include "./raytracing/shapes/triangle.h"

#include "./raytracing/material/material.h"
#include "./raytracing/manifest.h"

#include "./raytracing/lights/directional_light.h"
#include "./raytracing/lights/point_light.h"

#include <vector>
#include <optional>

namespace CRT
{
	class Scene
	{
	public:
		Scene() = default;

		void AddShape(Shape* _shape, Material* _material);
		void AddDirectionalLight(DirectionalLight _light);
		void AddPointLight(PointLight _light);

		float3 Intersect(Ray _r) const;
	private:
		float3 IntersectBounced(Ray _r, unsigned _remainingBounces) const;
		std::optional<Manifest> GetNearestIntersection(Ray _ray) const;
		float3 GetTotalLightContribution(const Manifest& _manifest) const;

		template<typename TLight>
		float3 GetLightContribution(const Manifest& _manifest, const TLight& _light) const
		{
			ShadowRay shadowRay = _light.ConstructShadowRay(_manifest);
			std::optional<Manifest> possible_blocker = GetNearestIntersection(shadowRay.Ray);
			if (!possible_blocker || possible_blocker->T >= shadowRay.MaxT)
			{
				return _light.GetLightContribution(_manifest);
			}
			return 0.0f;
		}
		
		const static float3 BackgroundColor;
		
		std::vector<Shape*>    m_Shapes;
		std::vector<Material*> m_Materials;
		std::vector<PointLight> m_PointLights;
		std::vector<DirectionalLight> m_DirectionalLights;
	};
}