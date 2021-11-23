#pragma once
#include "./raytracing/ray.h"
#include "./raytracing/shapes/shape.h"
#include "./raytracing/shapes/sphere.h"
#include "./raytracing/shapes/plane.h"

#include "./raytracing/material/material.h"
#include "./raytracing/manifest.h"

#include <vector>

namespace CRT
{
	class Scene
	{
	public:
		Scene() = default;

		void AddShape(Shape* _shape, Material* _material);

		Manifest Intersect(Ray _r);

		std::vector<Shape*>    m_Shapes;
		std::vector<Material*> m_Materials;
	};
}