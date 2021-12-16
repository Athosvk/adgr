#pragma once
#include <vector>
#include <./raytracing/shapes/triangle.h>
#include <./raytracing/aabb.h>
#include <memory>

namespace CRT
{
	using Primitive = Triangle;

	struct BVHNode
	{
		AABB Bounds;
		std::unique_ptr<BVHNode> Left;
		std::unique_ptr<BVHNode> Right;
		std::vector<Primitive>::const_iterator PrimitiveBegin;
		std::vector<Primitive>::const_iterator PrimitiveEnd;
	};

	struct SplitPoint
	{
		std::vector<Primitive>::const_iterator SplitPrimitive;
		float SplitCost;
	};

	class BVH
	{
	public:
		BVH(std::vector<Primitive> primitives);

	private:
		void Construct();
		BVHNode SplitNode(BVHNode node, std::vector<Primitive>::const_iterator _start, 
			std::vector<Primitive>::const_iterator _end);
		SplitPoint CalculateSplitpoint(std::vector<Primitive>::const_iterator _start, 
			std::vector<Primitive>::const_iterator _end) const;
		float GetCost(std::vector<Primitive>::const_iterator _start, 
			std::vector<Primitive>::const_iterator _end) const;
		AABB CalculateSmallestAABB(const std::vector<Primitive>& primitives) const;

		std::vector<Primitive> m_Primitives;
		BVHNode m_RootNode;
	};
}
