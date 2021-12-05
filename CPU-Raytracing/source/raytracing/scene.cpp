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
		m_DirectionalLights.emplace_back(std::move(_light));
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
				color = nearest->M->Texture->GetValue(nearest->UV);
			else
				color = nearest->M->Color;
			return color * GetTotalLightContribution(*nearest);
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

	float Scene::GetTotalLightContribution(const Manifest& _manifest) const
	{
		float totalLightContribution = 0.1f;
		for (const auto& directionalLight : m_DirectionalLights)
		{
			totalLightContribution += GetLightContribution(_manifest, directionalLight);
		}
		for (const auto& pointLight : m_PointLights)
		{
			totalLightContribution += GetLightContribution(_manifest, pointLight);
		}
		return std::min(totalLightContribution, 1.0f);
	}
}