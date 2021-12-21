#pragma once
#include "./raytracing/ray.h"
#include "./raytracing/shapes/shape.h"

#include "./raytracing/material/material.h"
#include "./raytracing/manifest.h"

#include "./raytracing/lights/directional_light.h"
#include "./raytracing/lights/spot_light.h"
#include "./raytracing/lights/point_light.h"
#include "./raytracing/bvh.h"

#include <vector>
#include <optional>

namespace CRT
{
	class Triangle;

	class Scene
	{
	public:
		Scene() = default;

		void AddShape(Shape* _shape, Material* _material);
		void AddTriangle(Triangle _triangle);
		void AddDirectionalLight(DirectionalLight _light);
		void AddSpotLight(SpotLight _light);
		void AddPointLight(PointLight _light);
		void ConstructBVH();

		float3 Intersect(Ray _r) const;
		void EnableBVH();
		void DisableBVH();
		bool IsBVHEnabled() const;
		uint64_t GetTriangleCount() const;
		uint64_t GetBHVNodeCount() const;
	private:
		float3 IntersectBounced(Ray _r, unsigned _remainingBounces) const;
		std::optional<Manifest> GetNearestIntersection(Ray _ray) const;
		float3 GetTotalLightContribution(const Manifest& _manifest) const;
		float3 GetReflectance(Ray _r, const Manifest& _manifest, unsigned _remainingBounces) const;

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
		
		std::optional<BVH> m_BVH;
		std::vector<Triangle> m_Triangles;
		std::vector<Shape*>    m_Shapes;
		std::vector<Material*> m_Materials;
		std::vector<PointLight> m_PointLights;
		std::vector<SpotLight> m_SpotLights;
		std::vector<DirectionalLight> m_DirectionalLights;
		bool m_UseBVH = true;
	};
}