#include "./scene.h"

#include <optional>

namespace CRT
{
	void Scene::AddShape(Shape* _shape, Material* _material)
	{
		m_Shapes.push_back(_shape);
		m_Materials.push_back(_material);
	}

	void Scene::AddDirectionalLight(DirectionalLight _light)
	{
		m_DirectionalLight = _light;
	}

	void Scene::AddPointLight(PointLight _light)
	{
		m_PointLights.emplace_back(std::move(_light));
	}

	float3 Scene::Intersect(Ray _r) const
	{
		std::optional<Manifest> nearest = GetNearestIntersection(_r);
		if (nearest)
		{ 
			float3 color;
			if (nearest->M->Texture != nullptr)
				color = nearest->M->Texture->GetValue(nearest->UV) ;
			else
				color = nearest->M->Color;
			return color * (GetLightContribution(*nearest));
		}
		return float3{ 0.0f, 0.0f, 0.0f };
	}

	std::optional<Manifest> Scene::GetNearestIntersection(Ray _ray) const
	{
		std::optional<Manifest> nearest;
		for (uint32_t i = 0; i < m_Shapes.size(); i++)
		{
			Manifest manifest;
			if (m_Shapes[i]->Intersect(_ray, manifest) && (!nearest || manifest.T < nearest->T))
			{
				manifest.M = m_Materials[i];
				nearest = manifest;
			}
		}
		return nearest;
	}

	float Scene::GetLightContribution(const Manifest& _manifest) const
	{
		float lighting = 0.0f;
		lighting = std::max(0.0f, _manifest.N.Dot(-m_DirectionalLight.Direction)) * m_DirectionalLight.Intensity;
		auto possible_blocker = GetNearestIntersection(m_DirectionalLight.ConstructShadowRay(_manifest));

		if (possible_blocker)
		{
			return 0.1f;
		}
		return lighting + 0.1f;
	}
}