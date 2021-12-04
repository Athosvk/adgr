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

	PointLight* Scene::AddPointLight(PointLight _light)
	{
		return m_PointLights.emplace_back(std::make_unique<PointLight>(std::move(_light))).get();
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
		float totalLightContribution = 0.1f;
		if (m_DirectionalLight)
		{
			auto shadowRay = m_DirectionalLight->ConstructShadowRay(_manifest);
			auto possible_blocker = GetNearestIntersection(shadowRay.Ray);
			if (!possible_blocker)
			{
				totalLightContribution += m_DirectionalLight->GetLightContribution(_manifest);
			}
		}
		for (const auto& pointLight : m_PointLights)
		{
			auto shadowRay = pointLight->ConstructShadowRay(_manifest);
			auto possible_blocker = GetNearestIntersection(shadowRay.Ray);
			if (!possible_blocker || possible_blocker->T >= shadowRay.MaxT)
			{
				totalLightContribution += pointLight->GetLightContribution(_manifest);
			}
		}
		return std::min(totalLightContribution, 1.0f);
	}
}