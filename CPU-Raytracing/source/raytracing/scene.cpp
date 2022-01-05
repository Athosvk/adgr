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

	void Scene::AddTriangle(Triangle _triangle, Material* _material)
	{
		m_Triangles.push_back(_triangle);
		m_TriangleMaterials.push_back(_material);
	}

	void Scene::AddDirectionalLight(DirectionalLight _light)
	{
		m_DirectionalLights.emplace_back(std::move(_light));
	}

	void Scene::AddSpotLight(SpotLight _light)
	{
		m_SpotLights.emplace_back(std::move(_light));
	}

	void Scene::AddPointLight(PointLight _light)
	{
		m_PointLights.emplace_back(std::move(_light));
	}

	void Scene::ConstructBVH()
	{
		m_BVH = std::make_unique<BVH>(m_Triangles);
	}

	float3 Scene::Intersect(Ray _r) const
	{
		return IntersectBounced(_r, 5);
	}

	void Scene::Intersect(const RayPacket& _r, float3* _ptr, int _id) const
	{
		IntersectBounced(_r, _ptr, _id);
	}

	void Scene::EnableBVH()
	{
		m_UseBVH = true;
	}

	void Scene::DisableBVH()
	{
		m_UseBVH = false;
	}

	void Scene::SetBVHDebugSetting(ETraversalDebugSetting _debugSetting)
	{
		m_DebugSetting = _debugSetting;
	}

	ETraversalDebugSetting Scene::GetBVHDebugSetting() const
	{
		return m_DebugSetting;
	}

	bool Scene::IsBVHEnabled() const
	{
		return m_UseBVH;
	}

	uint64_t Scene::GetTriangleCount() const
	{
		return m_Triangles.size();
	}

	uint64_t Scene::GetBHVNodeCount() const
	{
		return m_BVH->GetNodeCount();
	}

	float3 Scene::IntersectBounced(Ray _r, unsigned _remainingBounces) const
	{
		if (_remainingBounces == 0)
		{
			return BackgroundColor;
		}
		TraversalResult result = GetNearestIntersection(_r);
		std::optional<Manifest> nearest = result.Manifest;

		float3 debugColor = float3::Zero();
		if (m_UseBVH)
		{
			debugColor = float3(0.0f, result.Traversals / (float)m_BVH->GetDepth(), 0.0f);
		}
		float3 color = BackgroundColor;

		if (nearest && m_DebugSetting != ETraversalDebugSetting::TraversalOnly)
		{
			color = RenderObject(_r, *nearest, _remainingBounces);
			if (m_DebugSetting == ETraversalDebugSetting::Blend)
			{
				color += debugColor;
			}
		}
		// Only render bvh nodes, i.e. there was at least one traversal
		else if (m_DebugSetting != ETraversalDebugSetting::None && result.Traversals > 0)
		{
			color += debugColor;
		}
		// Make sure to clamp if the debug color caused it to exceed
		if (m_DebugSetting != ETraversalDebugSetting::None)
		{
			color = color.ComponentMin(float3::One());
		}
		return color;
	}

	void Scene::IntersectBounced(const RayPacket& _r, float3* _ptr, int _id) const
	{
		if (m_UseBVH)
		{
			TraversalResultPacket packetResult(16 * 16);
			m_BVH->GetNearestIntersection(_r, packetResult);

			uint32_t x, y;
			for (int i = 0; i < 16 * 16; i++)
			{
				// Retrieve morton index
				morton_to_xy(_id + i, &x, &y);

				//	Ray r(_r.O, _r.D[i]);
				//	std::optional<Manifest> nearest = GetNearestIntersection(r);
				if (packetResult.T[i] < 1000.0f)
				{
					float3 p = _r.O + _r.D[i] * packetResult.T[i];
					float3 n = m_Triangles[packetResult.ID[i]].N1;
					float2 uv = m_Triangles[packetResult.ID[i]].GetUV(p, n);

					float3 color;
					Material* mat = m_TriangleMaterials[packetResult.ID[i]];
					if (mat != nullptr)
					{
						if (mat->Texture != nullptr)
							color = mat->Texture->GetValue(uv);
						else
							color = mat->Color;
					}

					//	float3 lightdir = float3(0.0f, 1.0f, 1.0f).Normalize();
					//	float diff = std::max(n.Dot(lightdir), 0.0f);
					//	diff += 0.1f;
					//
					//	_ptr[x + y * 16] = float3(diff, diff, diff);

					float3 material_effect;

					Manifest m;
					m.N = n;
					m.T = packetResult.T[i];
					m.UV = uv;
					material_effect += GetTotalLightContribution(m);

					_ptr[x + y * 16] = color * material_effect;
				}
			}
		}
	}

	float3 Scene::RenderObject(Ray _r, const Manifest& _manifest, unsigned _remainingBounces) const
	{
		float3 object_color;
		if (_manifest.M->Texture != nullptr)
			object_color = _manifest.M->Texture->GetValue(_manifest.UV);
		else
			object_color = _manifest.M->Color;
		float specularity = _manifest.M->Specularity;

		float3 material_effect;
		const float MinLightingComponent = 0.001f;
		if (_manifest.M->type == Type::Basic)
		{
			if (specularity > MinLightingComponent)
			{
				material_effect += GetReflectance(_r, _manifest, _remainingBounces) * specularity;
			}

			float diffuseness = 1.0f - specularity;
			if (diffuseness > MinLightingComponent)
			{
				material_effect += GetTotalLightContribution(_manifest) * diffuseness;
			}
		}
		else if (_manifest.M->type == Type::Dielectric)
		{
			// Get the cosine of the angle between the normal and the incoming ray
			// by inverting the incoming ray's direction
			float cosIncoming = ((-_r.D)).Dot(_manifest.N);
			float3 normal = _manifest.N;
			bool front_face = cosIncoming > 0.0f;
			if (!front_face)
			{
				// Moving outside the object, so computed cosine and normal should be the other way
				normal = -normal;
				cosIncoming = -cosIncoming;
			}

			float n1 = 1.0f;
			float n2 = _manifest.M->RefractionIndex;
			if (!front_face)
				std::swap(n1, n2);

			float refractionIndexRatio = n1 / n2;
			float k = 1.0f - (refractionIndexRatio * refractionIndexRatio) * (1.0f - (cosIncoming * cosIncoming));
			float reflectance = 0.0f;

			// Not past critical angle, refract ray
			if (k >= 0.f)
			{
				// Fresnel
				float sinIncoming = sqrtf(1.0f - cosIncoming * cosIncoming);
				// Calculate from sin using Snell's law
				float cosOutgoing = sqrtf(1.0f - std::powf((refractionIndexRatio * sinIncoming), 2));
				float reflectanceSPolarized = std::powf((n1 * cosIncoming - n2 * cosOutgoing) /
					(n1 * cosIncoming + n2 * cosOutgoing), 2);
				float reflectancePPolarized = std::powf((n1 * cosOutgoing - n2 * cosIncoming) /
					(n1 * cosOutgoing + n2 * cosIncoming), 2);
				reflectance = 0.5f * (reflectanceSPolarized + reflectancePPolarized);
			}
			else
			{
				reflectance = 1.0f;
			}
			if (reflectance > MinLightingComponent)
			{
				material_effect += GetReflectance(_r, _manifest, _remainingBounces) * reflectance;
			}
			float transmittance = 1.0f - reflectance;
			if (transmittance > MinLightingComponent)
			{
				float3 refractionDirection = refractionIndexRatio * _r.D +
					normal * (refractionIndexRatio * cosIncoming - std::sqrtf(k));

				// Make sure the refraction ray doesn't self-intersect
				const float SelfIntersectionDelta = 0.001f;
				// Displace into opposite direction since we're moving into the new medium
				const float3 Displacement = SelfIntersectionDelta * normal;

				float3 transmittedColor = IntersectBounced(Ray(_manifest.IntersectionPoint - Displacement,
					refractionDirection), _remainingBounces - 1);
				if (!front_face)
				{
					// Beer's law. Divided by 5 to reduce the effect
					transmittedColor.x *= std::expf(-transmittedColor.x * (_manifest.T / 5));
					transmittedColor.y *= std::expf(-transmittedColor.y * (_manifest.T / 5));
					transmittedColor.z *= std::expf(-transmittedColor.z * (_manifest.T / 5));
				}
				material_effect += transmittedColor * transmittance;
			}
		}
		return material_effect * object_color;
	}

	TraversalResult Scene::GetNearestIntersection(Ray _ray) const
	{
		TraversalResult result;
		if (m_UseBVH)
		{
			result = m_BVH->GetNearestIntersection(_ray);
			if (result.Manifest)
			{
				result.Manifest->M = m_TriangleMaterials[result.Index];
			}
		}
		else
		{
			std::optional<Manifest> nearest;
			for (uint32_t i = 0; i < m_Triangles.size(); i++)
			{
				Manifest manifest;
				if (m_Triangles[i].Intersect(_ray, manifest) && (!nearest || manifest.T < nearest->T))
				{
					manifest.M = m_TriangleMaterials[i];
					nearest = manifest;
					result.Index = i;
				}
			}
			result.Manifest = std::move(nearest);
		}
		std::optional<Manifest> nearest = result.Manifest;
		for (uint32_t i = 0; i < m_Shapes.size(); i++)
		{
			Manifest manifest;
			if (m_Shapes[i]->Intersect(_ray, manifest) && (!nearest || manifest.T < nearest->T))
			{
				manifest.M = m_Materials[i];
				nearest = manifest;
				result.Index = i;
			}
		}
		result.Manifest = std::move(nearest);
		return result;
	}

	float3 Scene::GetTotalLightContribution(const Manifest& _manifest) const
	{
		float3 totalLightContribution = { 0.1f, 0.1f, 0.1f };
		for (const DirectionalLight& directionalLight : m_DirectionalLights)
		{
			totalLightContribution += GetLightContribution(_manifest, directionalLight);
		}
		for (const SpotLight& spotLight : m_SpotLights)
		{
			totalLightContribution += GetLightContribution(_manifest, spotLight);
		}
		for (const PointLight& pointLight : m_PointLights)
		{
			totalLightContribution += GetLightContribution(_manifest, pointLight);
		}
		return { std::min(totalLightContribution.x, 1.0f),
				std::min(totalLightContribution.y, 1.0f),
				std::min(totalLightContribution.z, 1.0f) };
	}

	float3 Scene::GetReflectance(Ray _r, const Manifest& _manifest, unsigned _remainingBounces) const
	{
		// Make sure the refraction ray doesn't self-intersect
		const float SelfIntersectionDelta = 0.001f;
		// Displace into opposite direction since we're moving into the new medium
		const float3 Displacement = SelfIntersectionDelta * _manifest.N;

		Ray reflectedRay = _r.Reflect(_manifest.N);
		reflectedRay.O = _manifest.IntersectionPoint;
		return IntersectBounced(reflectedRay, _remainingBounces - 1);
	}
}
