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

namespace CRT
{
	class Scene
	{
	public:
		Scene() = default;

		void AddShape(Shape* _shape, Material* _material);
		void AddDirectionalLight(DirectionalLight _light);
		void AddPointLight(PointLight _light);

		float3 Intersect(Ray _r);
	private:
		float GetLightContribution(const Manifest& _manifest) const;

		std::vector<Shape*>    m_Shapes;
		std::vector<Material*> m_Materials;
		std::vector<PointLight> m_PointLights;
		DirectionalLight m_DirectionalLight;
	};
}