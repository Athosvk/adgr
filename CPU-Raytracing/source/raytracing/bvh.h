#pragma once
#include <vector>
#include <./raytracing/shapes/triangle.h>
#include <./raytracing/aabb.h>
#include <memory>
#include <./raytracing/ray.h>
#include <optional>

namespace CRT
{
	using Primitive = Triangle;

	struct BVHNode
	{
		AABB Bounds;
		std::unique_ptr<BVHNode> Left;
		std::unique_ptr<BVHNode> Right;
		std::vector<Primitive> Primitives;
	};

	struct SplitPoint
	{
		std::vector<Primitive>::const_iterator SplitPrimitive;
		float SplitCost = 0.0f;
	};

	struct TraversalResult
	{
		std::vector<Primitive> Primitives;
		uint32_t Traversals;
	};

	class BVH
	{
	public:
		BVH(std::vector<Primitive> primitives);

		TraversalResult Traverse(const Ray& ray) const;
	private:
		TraversalResult TraverseNode(const Ray& ray, const BVHNode* parentNode) const;
		void Construct();
		BVHNode SplitNode(BVHNode node, std::vector<Primitive> primitives) const;
		SplitPoint CalculateSplitpoint(std::vector<Primitive>& _primitives) const;
		float GetCost(std::vector<Primitive>::const_iterator _start, 
			std::vector<Primitive>::const_iterator _end) const;
		AABB CalculateSmallestAABB(std::vector<Primitive>::const_iterator _start, 
			std::vector<Primitive>::const_iterator _end) const;

		std::vector<Primitive> m_Primitives;
		BVHNode m_RootNode;
	};
}
