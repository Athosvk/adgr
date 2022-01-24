#include "Mesh.h"

namespace CRT
{
	Mesh::Mesh(std::vector<Triangle> _triangles, Material* _material) : 
		m_BVH(_triangles),
		m_Triangles(_triangles),
		m_Material(_material)
	{
	}

	TraversalResult Mesh::FindBVHIntersection(const Ray& _ray) const
	{
		TraversalResult result = m_BVH.GetNearestIntersection(_ray);
		if (result.Manifest)
		{
			result.Manifest->M = m_Material;
		}
		return result;
	}
		
	std::optional<Manifest> Mesh::FindIntersection(const Ray& _ray) const
	{
		std::optional<Manifest> nearest;
		for (uint32_t i = 0; i < m_Triangles.size(); i++)
		{
			Manifest manifest;
			if (m_Triangles[i].IntersectDisplaced(_ray, manifest, m_Material->HeightMap) 
				&& (!nearest || manifest.T < nearest->T))
			{
				manifest.M = m_Material;
				nearest = std::move(manifest);
			}
		}
		return nearest;
	}

	uint64_t Mesh::GetTriangleCount() const
	{
		return m_Triangles.size();
	}

	uint64_t Mesh::GetBVHNodeCount() const
	{
		return m_BVH.GetNodeCount();
	}
}
