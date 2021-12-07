#include "./scene.h"
#include <./core/graphics/color3.h>

#include <optional>
#include <algorithm>
#include <cmath>

namespace CRT
{
	const float3 Scene::BackgroundColor = float3(0.4f, 0.4f, 0.4f);

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
		return IntersectBounced(_r, 5);
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
				// Get the cosine of the angle between the normal and the incoming ray
				// by inverting the incoming ray's direction
				float cosIncoming = ((-_r.D)).Dot(nearest->N);
				float3 normal = nearest->N;
				bool front_face = cosIncoming > 0.0f;
				if (!front_face)
				{
					// Moving outside the object, so computed cosine and normal should be the other way
					normal = -normal;
					cosIncoming = -cosIncoming;
				}

				float n1 = 1.0f;
				float n2 = nearest->M->RefractionIndex;
				if (!front_face)
					std::swap(n1, n2);

				float refractionIndexRatio = n1 / n2;
				float k = 1.0f - (refractionIndexRatio * refractionIndexRatio) * (1.0f - (cosIncoming * cosIncoming));
				float reflectance = 0.0f;

				// Make sure the refraction ray doesn't self-intersect
				const float SelfIntersectionDelta = 0.001f;
				// Displace into opposite direction since we're moving into the new medium
				const float3 Displacement = SelfIntersectionDelta * normal;

				// Not past critical angle, refract ray
				if (k >= 0.f)
				{
					// Fresnel
					float sinIncoming = sqrtf(1.0f - cosIncoming * cosIncoming);
					// Calculate from sin using Snell's law
					float cosOutgoing = sqrtf(1.0f - std::powf((refractionIndexRatio * sinIncoming), 2));
					float reflectanceSPolarized = std::pow((n1 * cosIncoming - n2 * cosOutgoing) / 
														   (n1 * cosIncoming + n2 * cosOutgoing), 2);
					float reflectancePPolarized = std::pow((n1 * cosOutgoing - n2 * cosIncoming) / 
														   (n1 * cosOutgoing + n2 * cosIncoming), 2);
					reflectance = 0.5f * (reflectanceSPolarized + reflectancePPolarized);
				}
				else
				{
					reflectance = 1.0f;
				}
				if (reflectance > 0.001f)
				{
					Ray reflectedRay = _r.Reflect(nearest->N);
					reflectedRay.O = nearest->IntersectionPoint + Displacement;
					material_effect += IntersectBounced(reflectedRay, _remainingBounces - 1) * reflectance;
				}
				float transmittance = 1.0f - reflectance;
				if (transmittance > 0.001f)
				{
					float3 refractionDirection = refractionIndexRatio * _r.D + 
						normal * (refractionIndexRatio * cosIncoming - std::sqrtf(k));

					float3 transmittedColor = IntersectBounced(Ray(nearest->IntersectionPoint - Displacement,
						refractionDirection), _remainingBounces - 1);
					if (!front_face)
					{
						// Beer's law
						transmittedColor.x *= std::expf(-transmittedColor.x * nearest->T);
						transmittedColor.y *= std::expf(-transmittedColor.y * nearest->T);
						transmittedColor.z *= std::expf(-transmittedColor.z * nearest->T);
					}
					material_effect += transmittedColor * transmittance;
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