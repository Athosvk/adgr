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
			SplitPoint splitPoint = CalculateSplitpoint(_primitives);

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

	SplitPoint BVH::CalculateSplitpoint(std::vector<Primitive>& _primitives) const
	{
		std::array<SplitPoint, 3> splitPoints;
		size_t Bins = 16u;

		for (uint32_t i = 0u; i < 3; i++)
		{
			splitPoints[i].SplitCost = std::numeric_limits<float>::infinity();
			std::sort(_primitives.begin(), _primitives.end(), [i](const Primitive& first, const Primitive& second) {
				// TO-DO: Maybe use the barycenter instead
				return first.V0.f[i] < second.V0.f[i];
			});

			// Sorted, so the width of this dimension is the difference between the first
			// and last element
			float splitWidth = _primitives.back().V0.f[i] - _primitives.front().V0.f[i];
			if (splitWidth <= 0.0001f)
			{
				// Nothing to split along this axis
				continue;
			}

			auto splitPrimitive = _primitives.cbegin();

			// Don't iterate over the first and last bins, so that we're guaranteed to have at least one primitive
			// on both sides
			for (size_t bin = 1u; bin < Bins - 1; bin++)
			{
				// Place the separator at Bins number of locations to try as split points
				auto separator = (splitWidth / Bins) * bin + _primitives.front().V0.f[i];

				// Find the first primitive past the separator, i.e. where the "bin ends"
				splitPrimitive = std::find_if(splitPrimitive, _primitives.cend(), [separator, i](const Primitive& primitive)
				{
					return primitive.V0.f[i] >= separator;
				});

				auto costLeft = GetCost(_primitives.begin(), splitPrimitive);
				auto costRight = GetCost(splitPrimitive, _primitives.end());
				auto totalCost = costLeft + costRight;
				if (totalCost < splitPoints[i].SplitCost)
				{
					splitPoints[i] = { splitPrimitive, totalCost };
				}
			}
		}
		return *std::min_element(splitPoints.begin(), splitPoints.end(),
			[](const SplitPoint& left, const SplitPoint& right)
		{
			return left.SplitCost < right.SplitCost;
		});
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
