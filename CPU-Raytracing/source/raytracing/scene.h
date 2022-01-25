#pragma once
#include "./raytracing/ray.h"
#include "./raytracing/shapes/shape.h"

#include "./raytracing/material/material.h"
#include "./raytracing/manifest.h"

#include "./raytracing/lights/directional_light.h"
#include "./raytracing/lights/spot_light.h"
#include "./raytracing/lights/point_light.h"
#include "./raytracing/bvh.h"
#include <./raytracing/shapes/mesh.h>

#include <vector>
#include <optional>
#include <memory>

namespace CRT
{
	class Triangle;

	enum class ETraversalDebugSetting
	{
		None,
		/* Blend it with the scene coloring */
		Blend,
		/* Only debug draw the traversals that did not hit any objects, i.e. draw objects normally. Appear to be on top of the debug BVH */
		ExcludeHits,
		/* Only debug draw, do not draw objects */
		TraversalOnly
	};

	class Scene
	{
	public:
		Scene() = default;

		void AddShape(Shape* _shape, Material* _material);
		void AddMesh(Mesh _mesh);
		void AddDirectionalLight(DirectionalLight _light);
		void AddSpotLight(SpotLight _light);
		void AddPointLight(PointLight _light);

		float3 Intersect(Ray _r) const;
		void Intersect(const RayPacket& _r, float3* _ptr, int _id) const;

		void EnableBVH();
		void DisableBVH();
		void SetBVHDebugSetting(ETraversalDebugSetting _debugSetting);
		ETraversalDebugSetting GetBVHDebugSetting() const;
		bool IsBVHEnabled() const;
		uint64_t GetTriangleCount() const;
		uint64_t GetBHVNodeCount() const;
	private:
		float3 IntersectBounced(Ray _r, unsigned _remainingBounces) const;
		void IntersectBounced(const RayPacket& _r, float3* _ptr, int _id) const;
		float3 RenderObject(Ray _r, const Manifest& _manifest, unsigned _remainingBounces) const;

		TraversalResult GetNearestIntersection(Ray _ray) const;
		float3 GetTotalLightContribution(const Manifest& _manifest) const;
		float3 GetReflectance(Ray _r, const Manifest& _manifest, unsigned _remainingBounces) const;

		template<typename TLight>
		float3 GetLightContribution(const Manifest& _manifest, const TLight& _light) const
		{
			auto contribution = _light.GetLightContribution(_manifest);
			if (contribution > 0.001f)
			{
				ShadowRay shadowRay = _light.ConstructShadowRay(_manifest);
				std::optional<Manifest> possible_blocker = GetNearestIntersection(shadowRay.Ray).Manifest;
				if (!possible_blocker || possible_blocker->T >= shadowRay.MaxT)
				{
					return contribution;
				}
			}
			return 0.0f;
		}

		const static float3 BackgroundColor;

		std::vector<std::unique_ptr<Mesh>> m_Meshes;
		std::vector<Shape*>    m_Shapes;
		std::vector<Material*> m_Materials;
		std::vector<PointLight> m_PointLights;
		std::vector<SpotLight> m_SpotLights;
		std::vector<DirectionalLight> m_DirectionalLights;
		ETraversalDebugSetting m_DebugSetting = ETraversalDebugSetting::None;
		bool m_UseBVH = true;
	};
}
