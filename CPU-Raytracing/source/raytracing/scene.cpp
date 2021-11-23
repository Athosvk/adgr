#include "./scene.h"

namespace CRT
{
	void Scene::AddShape(Shape* _shape, Material* _material)
	{
		m_Shapes.push_back(_shape);
		m_Materials.push_back(_material);
	}

	Manifest Scene::Intersect(Ray _r)
	{
		Manifest m;
		for (uint32_t i = 0; i < m_Shapes.size(); i++)
		{
			if (m_Shapes[i]->Intersect(_r, m))
				m.M = m_Materials[i];
		}
		
		return m;
	}
}