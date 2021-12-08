#pragma once
#include "./raytracing/scene.h"

namespace CRT
{
	class ModelLoading
	{
	public:
		static void LoadModel(Scene* _scene, Material* material, float3 _offset, const std::string& _filepath);
	};
}