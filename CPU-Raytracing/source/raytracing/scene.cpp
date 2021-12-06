#include "./scene.h"
#include <./core/graphics/color3.h>

#include <optional>
#include <algorithm>
#include <cmath>

namespace CRT
{
	const float3 Scene::BackgroundColor = Color::Black;

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
		return IntersectBounced(_r, 2);
	}

	float3 Scene::IntersectBounced(Ray _r, unsigned _remainingBounces) const
	{
		if (_remainingBounces == 0)
		{
			return BackgroundColor;
		}
		std::optional<Manifest> nearest = GetNearestIntersection(_r);
		if (nearest)
		{ 
			float3 object_color;
			if (nearest->M->Texture != nullptr)
				object_color = nearest->M->Texture->GetValue(nearest->UV);
			else
				object_color = nearest->M->Color;
			float specularity = nearest->M->Specularity;

			float3 material_effect;
			if (nearest->M->type == Type::Basic)
			{
				const float MinLightingComponent = 0.01f;
				if (specularity > MinLightingComponent)
				{
					Ray reflectedRay = _r.Reflect(nearest->N);
					reflectedRay.O = nearest->IntersectionPoint;
					material_effect += IntersectBounced(reflectedRay, _remainingBounces - 1) * specularity;
				}

				float diffuseness = 1.0f - specularity;
				if (diffuseness > MinLightingComponent)
				{
					material_effect += GetTotalLightContribution(*nearest) * diffuseness;
				}
			}
			else if (nearest->M->type == Type::Dielectric)
			{
				Ray scattered_ray = _r;
				// Get the cosine of the angle between the normal and the incoming ray
				// by inverting the incoming ray's direction
				float cosTheta = (-_r.D.Normalize()).Dot(nearest->N);
				bool front_face = cosTheta > 0.0f;

				float refractionIndexRatio = front_face ? 1.0f / nearest->M->RefractionIndex : nearest->M->RefractionIndex;
				float k = 1 - (refractionIndexRatio * refractionIndexRatio) * (1 - (cosTheta * cosTheta));
				float reflectance = 0.0f;
				// Not past critical angle, refract ray
				if (k >= 0.f)
				{
					float3 refractionDirection = refractionIndexRatio * _r.D + 
						nearest->N * (refractionIndexRatio * cosTheta - std::sqrt(k));
					material_effect = IntersectBounced(Ray(nearest->IntersectionPoint,
						refractionDirection), _remainingBounces);
				}
				else
				{
					reflectance = 1.0f;
					Ray reflectedRay = _r.Reflect(nearest->N);
					reflectedRay.O = nearest->IntersectionPoint;
					material_effect = IntersectBounced(reflectedRay, _remainingBounces - 1);
				}
			}
			return material_effect * object_color;
		}
		return BackgroundColor;
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

	float3 Scene::GetTotalLightContribution(const Manifest& _manifest) const
	{
		float3 totalLightContribution = { 0.1f, 0.1f, 0.1f };
		for (const DirectionalLight& directionalLight : m_DirectionalLights)
		{
			totalLightContribution += GetLightContribution(_manifest, directionalLight);
		}
		for (const PointLight& pointLight : m_PointLights)
		{
			totalLightContribution += GetLightContribution(_manifest, pointLight);
		}
		return std::min(totalLightContribution, float3::One());
		return { std::min(totalLightContribution.x, 1.0f),
				std::min(totalLightContribution.y, 1.0f),
				std::min(totalLightContribution.z, 1.0f) };
	}
}