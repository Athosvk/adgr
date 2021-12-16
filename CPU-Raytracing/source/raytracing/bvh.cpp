#include "bvh.h"

#include <algorithm>
#include <memory>

namespace CRT
{
	BVH::BVH(std::vector<Primitive> _primitives) :
		m_Primitives(std::move(_primitives))
	{
	}

	void BVH::Construct()
	{
		BVHNode root;
		root.Bounds = CalculateSmallestAABB(m_Primitives);
		m_RootNode = SplitNode(std::move(root), m_Primitives.begin(), m_Primitives.end());
	}

	BVHNode BVH::SplitNode(BVHNode node, std::vector<Primitive>::const_iterator _start,
		std::vector<Primitive>::const_iterator _end)
	{
		SplitPoint splitPoint = CalculateSplitpoint(_start, _end);
		float parentCost = GetCost(_start, _end);
		if (splitPoint.SplitCost < parentCost)
		{
			BVHNode left;
			left.Bounds = CalculateSmallestAABB(std::vector<Primitive>(_start, splitPoint.SplitPrimitive));
			left = SplitNode(std::move(left), _start, splitPoint.SplitPrimitive);
			node.Left = std::make_unique<BVHNode>(std::move(left));

			BVHNode right;
			right.Bounds = CalculateSmallestAABB(std::vector<Primitive>(splitPoint.SplitPrimitive + 1, _end));
			right = SplitNode(std::move(right), splitPoint.SplitPrimitive + 1, _end);
			node.Right = std::make_unique<BVHNode>(std::move(right));
		}
		else
		{
			node.PrimitiveBegin = _start;
			node.PrimitiveEnd = splitPoint.SplitPrimitive;
		}
		return node;
	}

	SplitPoint BVH::CalculateSplitpoint(std::vector<Primitive>::const_iterator _start, 
		std::vector<Primitive>::const_iterator _end) const
	{
		std::vector<Primitive> primitives(_start, _end);
		std::sort(primitives.begin(), primitives.end(), [](const Primitive& first, const Primitive& second) {
			// TO-DO: Maybe use the barycenter instead
			return first.V0.x < second.V0.x;
		});

		SplitPoint splitPoint;
		for (auto it = primitives.cbegin(); it != primitives.cend(); it++)
		{
			auto costLeft = GetCost(primitives.cbegin(), it);
			auto costRight = GetCost(it, primitives.cend());
			auto totalCost = costLeft + costRight;
			if (totalCost < splitPoint.SplitCost)
			{
				splitPoint = { it, totalCost };
			}
		}
		return splitPoint;
	}

	float BVH::GetCost(std::vector<Primitive>::const_iterator _start, 
		std::vector<Primitive>::const_iterator _end) const
	{
		return CalculateSmallestAABB(std::vector<Primitive>(_start, _end)).GetSurfaceArea()
			* std::distance(_start, _end);
	}

	AABB BVH::CalculateSmallestAABB(const std::vector<Primitive>& primitives) const
	{
		AABB boundingBox;
		for (const Primitive& primitive : primitives)
		{
			boundingBox.Min = std::min({ boundingBox.Min, primitive.V0, primitive.V1, primitive.V2 });
			boundingBox.Max = std::max({ boundingBox.Max, primitive.V0, primitive.V1, primitive.V2 });
		}
		return boundingBox;
	}
}
