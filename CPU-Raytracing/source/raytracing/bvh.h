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
	using PrimitiveIndex = uint32_t;

	struct PrimitiveRange
	{
		PrimitiveIndex FirstPrimitiveIndex = 0;
		uint32_t Count = 0;
	};

	struct BVHNode
	{
		AABB Bounds;
		uint32_t Count = 0;
		union
		{
			uint32_t Left = 0;
			PrimitiveIndex First;
		};
	};

	struct SplitPoint
	{
		std::vector<PrimitiveIndex> Left;
		std::vector<PrimitiveIndex> Right;
		float SplitCost = 0.0f;
	};

	struct TraversalResult
	{
		std::vector<PrimitiveRange> Primitives;
		uint32_t Traversals = 0;
	};

	class BVH
	{
	public:
		BVH(std::vector<Primitive> primitives);
		
		std::vector<Primitive> Traverse(const Ray& ray) const;
	private:
		TraversalResult TraverseNode(const Ray& ray, const BVHNode& parentNode) const;
		void Construct();
		BVHNode SplitNode(BVHNode node, const std::vector<PrimitiveIndex>& _range);
		SplitPoint CalculateSplitpoint(const std::vector<PrimitiveIndex>& _range) const;
		float GetCost(std::vector<PrimitiveIndex>::const_iterator _start, 
			std::vector<PrimitiveIndex>::const_iterator _end) const;
		AABB CalculateSmallestAABB(std::vector<PrimitiveIndex>::const_iterator _start, 
			std::vector<PrimitiveIndex>::const_iterator _end) const;

		std::vector<Primitive> m_Primitives;
		std::vector<uint32_t> m_PrimitiveIndices;
		std::vector<BVHNode> m_Nodes;
		BVHNode m_RootNode;
	};
}
