#pragma once
#include "./raytracing/ray.h"
#include "./raytracing/shapes/shape.h"
#include "./raytracing/shapes/sphere.h"
#include "./raytracing/shapes/plane.h"

#include "./raytracing/material/material.h"
#include "./raytracing/manifest.h"

#include "./raytracing/lights/directional_light.h"
#include "./raytracing/lights/point_light.h"

#include <vector>
#include <memory>
#include <optional>

namespace CRT
{
	class Scene
	{
	public:
		Scene() = default;

		void AddShape(Shape* _shape, Material* _material);
		void AddDirectionalLight(DirectionalLight _light);
		PointLight* AddPointLight(PointLight _light);

		float3 Intersect(Ray _r) const;
	private:
		std::optional<Manifest> GetNearestIntersection(Ray _ray) const;
		float GetLightContribution(const Manifest& _manifest) const;

		std::vector<Shape*>    m_Shapes;
		std::vector<Material*> m_Materials;
		std::vector<std::unique_ptr<PointLight>> m_PointLights;
		std::optional<DirectionalLight> m_DirectionalLight;
	};
}