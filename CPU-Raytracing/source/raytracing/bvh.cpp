#include "bvh.h"

#include <algorithm>
#include <memory>
#include <array>

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

	TraversalResult BVH::Traverse(const Ray& _ray) const
	{
		return TraverseNode(_ray, &m_RootNode);
	}

	TraversalResult BVH::TraverseNode(const Ray& _ray, const BVHNode* _parentNode) const
	{
		if (!_parentNode->Primitives.empty())
			return { _parentNode->Primitives, 0 };
		TraversalResult result;
		if (_parentNode->Left->Bounds.Intersects(_ray))
		{
			auto resultLeft = TraverseNode(_ray, _parentNode->Left.get());
			result.Primitives.insert(result.Primitives.begin(), resultLeft.Primitives.begin(),
				resultLeft.Primitives.end());
			result.Traversals = resultLeft.Traversals + 1;
		}
		if (_parentNode->Right->Bounds.Intersects(_ray))
		{
			auto resultRight = TraverseNode(_ray, _parentNode->Right.get());
			result.Primitives.insert(result.Primitives.begin(), resultRight.Primitives.begin(),
				resultRight.Primitives.end());
			result.Traversals = std::max(resultRight.Traversals + 1, result.Traversals);
		}
		return result;
	}

	BVHNode BVH::SplitNode(BVHNode _node, std::vector<Primitive> _primitives) const
	{
		if (_primitives.size() > 1)
		{
			float parentCost = GetCost(_primitives.begin(), _primitives.end());
			std::array<SplitPoint, 3> splitPoints;
			for (uint32_t i = 0; i < 3; i++)
			{
				splitPoints[i].SplitCost = std::numeric_limits<float>::infinity();
				std::sort(_primitives.begin(), _primitives.end(), [i](const Primitive& first, const Primitive& second) {
					// TO-DO: Maybe use the barycenter instead
					return first.V0.f[i] < second.V0.f[i];
				});

				// Sorted, so the width of this dimension is equal to the difference between the first
				// and last element
				float splitWidth = _primitives.back().V0.f[i] - _primitives.front().V0.f[i];
				
				// No split on this axis possible otherwise
				if (splitWidth > 0.0001f)
				{
					splitPoints[i] = CalculateSplitpoint(_primitives.begin(), _primitives.end(),
						splitWidth, i);
				}
			}

			SplitPoint splitPoint = *std::min_element(splitPoints.begin(), splitPoints.end(),
				[](const SplitPoint& left, const SplitPoint& right)
			{
				return left.SplitCost < right.SplitCost;
			});
			auto TraversalCost = 10.0f;
			if (splitPoint.SplitCost + TraversalCost < parentCost)
			{
				BVHNode left;
				left.Bounds = CalculateSmallestAABB(_primitives.begin(), splitPoint.SplitPrimitive);
				left = SplitNode(std::move(left), std::vector<Primitive>(_primitives.cbegin(), splitPoint.SplitPrimitive));
				_node.Left = std::make_unique<BVHNode>(std::move(left));

				BVHNode right;
				right.Bounds = CalculateSmallestAABB(splitPoint.SplitPrimitive, _primitives.end());
				right = SplitNode(std::move(right), std::vector<Primitive>(splitPoint.SplitPrimitive, _primitives.cend()));
				_node.Right = std::make_unique<BVHNode>(std::move(right));
				return _node;
			}
		}
		_node.Primitives = std::move(_primitives);
		return _node;
	}

	SplitPoint BVH::CalculateSplitpoint(std::vector<Primitive>::const_iterator _start,
		std::vector<Primitive>::const_iterator _end, float _totalWidth, uint32_t dimension) const
	{
		SplitPoint splitPoint;
		splitPoint.SplitCost = std::numeric_limits<float>::infinity();

		size_t Bins = 16u;
		auto splitPrimitive = _start;

		// Don't iterate over the first and last bins, so that we're guaranteed to have at least one primitive
		// on both sides
		for (size_t bin = 1u; bin < Bins - 1; bin++)
		{
			// Place the separator at Bins number of locations to try as split points
			auto separator = (_totalWidth / Bins) * bin + _start->V0.f[dimension];

			// Find the first primitive past the separator, i.e. where the "bin ends"
			splitPrimitive = std::find_if(splitPrimitive, _end, [separator, dimension](Primitive primitive) 
			{
				return primitive.V0.f[dimension] >= separator; 
			});

			auto costLeft = GetCost(_start, splitPrimitive);
			auto costRight = GetCost(splitPrimitive, _end);
			auto totalCost = costLeft + costRight;
			if (totalCost < splitPoint.SplitCost)
			{
				splitPoint = { splitPrimitive, totalCost };
			}
		}
		//for (auto it = _start; it != _end; it++)
		//{
		//	auto costLeft = GetCost(_start, it + 1);
		//	auto costRight = GetCost(it, _end);
		//	auto totalCost = costLeft + costRight;
		//	if (totalCost < splitPoint.SplitCost)
		//	{
		//		splitPoint = { it, totalCost };
		//	}
		//}
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
