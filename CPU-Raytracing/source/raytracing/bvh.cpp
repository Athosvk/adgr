#include "bvh.h"

#include <algorithm>
#include <memory>
#include <queue>

namespace CRT
{
	BVH::BVH(std::vector<Primitive> _primitives) :
		m_Primitives(std::move(_primitives))
	{
		Construct();
	}

	void BVH::Construct()
	{
		BVHNode root;
		root.Bounds = CalculateSmallestAABB(m_Primitives.begin(), m_Primitives.end());
		m_RootNode = SplitNode(std::move(root), m_Primitives);
	}

	std::vector<Primitive> BVH::Traverse(const Ray& ray) const
	{
		std::vector<Primitive> primitives;
		std::queue<const BVHNode*> toVisit;
		toVisit.push(&m_RootNode);
		while(!toVisit.empty())
		{
			auto current = toVisit.front();
			toVisit.pop();
			if (current->Primitives.size() > 0)
			{
				primitives.insert(primitives.end(), current->Primitives.begin(), current->Primitives.end());
			}
			else
			{
				if (current->Left->Bounds.Intersects(ray))
				{
					toVisit.push(current->Left.get());
				}
				if (current->Right->Bounds.Intersects(ray))
				{
					toVisit.push(current->Right.get());
				}
			}
		} 
		return primitives;
	}

	BVHNode BVH::SplitNode(BVHNode node, std::vector<Primitive> primitives) const
	{
		std::sort(primitives.begin(), primitives.end(), [](const Primitive& first, const Primitive& second) {
			// TO-DO: Maybe use the barycenter instead
			return first.V0.x < second.V0.x;
		});
		SplitPoint splitPoint = CalculateSplitpoint(primitives.begin(), primitives.end());
		float parentCost = GetCost(primitives.begin(), primitives.end());

		if (splitPoint.SplitCost < parentCost)
		{
			BVHNode left;
			left.Bounds = CalculateSmallestAABB(primitives.begin(), splitPoint.SplitPrimitive + 1);
			left = SplitNode(std::move(left), std::vector<Primitive>(primitives.cbegin(), splitPoint.SplitPrimitive + 1));
			node.Left = std::make_unique<BVHNode>(std::move(left));

			BVHNode right;
			right.Bounds = CalculateSmallestAABB(splitPoint.SplitPrimitive + 1, primitives.end());
			right = SplitNode(std::move(right), std::vector<Primitive>(splitPoint.SplitPrimitive + 1, primitives.cend()));
			node.Right = std::make_unique<BVHNode>(std::move(right));
		}
		else
		{
			node.Primitives = std::move(primitives);
		}
		return node;
	}

	SplitPoint BVH::CalculateSplitpoint(std::vector<Primitive>::const_iterator _start, 
		std::vector<Primitive>::const_iterator _end) const
	{
		SplitPoint splitPoint;
		splitPoint.SplitCost = std::numeric_limits<float>::infinity();
		for (auto it = _start; it != _end; it++)
		{
			auto costLeft = GetCost(_start, it + 1);
			auto costRight = GetCost(it, _end);
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
		return CalculateSmallestAABB(_start, _end).GetSurfaceArea()
			* std::distance(_start, _end);
	}

	AABB BVH::CalculateSmallestAABB(std::vector<Primitive>::const_iterator _start, 
			std::vector<Primitive>::const_iterator _end) const
	{
		auto it = _start;
		AABB boundingBox{ float3::ComponentMin({ it->V0, it->V1, it->V2 }),
						  float3::ComponentMax({ it->V0, it->V1, it->V2 }) };
		it++;
		for (; it != _end; it++)
		{
			boundingBox.Min = float3::ComponentMin({ boundingBox.Min, it->V0, it->V1, it->V2 });
			boundingBox.Max = float3::ComponentMax({ boundingBox.Max, it->V0, it->V1, it->V2 });
		}
		return boundingBox;
	}
}
