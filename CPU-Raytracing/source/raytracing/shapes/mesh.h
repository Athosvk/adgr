#pragma once
#include <./raytracing/bvh.h>
#include <vector>

namespace CRT
{
	class Material;

	class Mesh
	{
	public:
		Mesh(std::vector<Triangle> _triangles, Material* _material);

		TraversalResult FindBVHIntersection(const Ray& _ray) const;
		std::optional<Manifest> FindIntersection(const Ray& _ray) const;
		uint64_t GetTriangleCount() const;
		uint64_t GetBVHNodeCount() const;
	private:
		BVH m_BVH;
		std::vector<Triangle> m_Triangles;
		Material* m_Material = nullptr;
	};
}
