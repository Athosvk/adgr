#include "bvh.h"

#include <algorithm>
#include <memory>
#include <array>

namespace CRT
{
	BVH::BVH(std::vector<Primitive> _primitives) :
		m_Primitives(_primitives)
	{
		Construct();
	}

	void BVH::Construct()
	{
		BVHNode root;
		std::vector<PrimitiveIndex> indices;
		indices.reserve(m_Primitives.size());
		for (uint32_t i = 0u; i < uint32_t(m_Primitives.size()); i++)
		{
			indices.emplace_back(i);
		}
		root.Bounds = CalculateSmallestAABB(indices.cbegin(), indices.cend());
		m_RootNode = SplitNode(std::move(root), indices);
	}

	std::vector<Primitive> BVH::Traverse(const Ray& _ray) const
	{
		if (!m_RootNode.Bounds.Intersects(_ray))
		{
			return {};
		}
		auto traversalResult = TraverseNode(_ray, m_RootNode);
		std::vector<Primitive> primitives;
		for (auto primitiveRange : traversalResult.Primitives)
		{
			for (uint32_t i = 0u; i < primitiveRange.Count; i++)
			{
				primitives.emplace_back(m_Primitives[m_PrimitiveIndices[size_t(primitiveRange.FirstPrimitiveIndex) + i]]);
			}
		}
		return primitives;
	}

	uint64_t BVH::GetNodeCount() const
	{
		return m_Nodes.size();
	}

	TraversalResult BVH::TraverseNode(const Ray& _ray, const BVHNode& _parentNode) const
	{
		if (_parentNode.Count > 0)
			return { { { _parentNode.First, _parentNode.Count } }, 0 };
		TraversalResult result;
		const auto& leftNode = m_Nodes[_parentNode.Left];
		if (leftNode.Bounds.Intersects(_ray))
		{
			auto resultLeft = TraverseNode(_ray, leftNode);
			result.Primitives.insert(result.Primitives.begin(), resultLeft.Primitives.begin(),
				resultLeft.Primitives.end());
			result.Traversals = resultLeft.Traversals + 1;
		}
		const auto& rightNode = m_Nodes[_parentNode.Left + 1];
		if (rightNode.Bounds.Intersects(_ray))
		{
			auto resultRight = TraverseNode(_ray, rightNode);
			result.Primitives.insert(result.Primitives.begin(), resultRight.Primitives.begin(),
				resultRight.Primitives.end());
			result.Traversals = std::max(resultRight.Traversals + 1, result.Traversals);
		}
		return result;
	}

	BVHNode BVH::SplitNode(BVHNode _node, const std::vector<PrimitiveIndex>& _primitiveRange)
	{
		if (_primitiveRange.size() > 1)
		{
			float parentCost = GetCost(_primitiveRange.cbegin(), _primitiveRange.cend());
			SplitPoint splitPoint = CalculateSplitpoint(_primitiveRange);

			auto TraversalCost = 10.0f;
			if (splitPoint.SplitCost + TraversalCost < parentCost)
			{
				BVHNode left;
				left.Bounds = CalculateSmallestAABB(splitPoint.Left.begin(), splitPoint.Left.end());
				left = SplitNode(left, splitPoint.Left);

				BVHNode right;
				right.Bounds = CalculateSmallestAABB(splitPoint.Right.begin(), splitPoint.Right.end());
				right = SplitNode(right, splitPoint.Right);

				_node.Left = uint32_t(m_Nodes.size());
				m_Nodes.emplace_back(std::move(left));
				m_Nodes.emplace_back(std::move(right));
				return _node;
			}
		}
		_node.First = m_PrimitiveIndices.size();
		m_PrimitiveIndices.insert(m_PrimitiveIndices.end(), _primitiveRange.begin(), _primitiveRange.end());
		_node.Count = _primitiveRange.size();
		return _node;
	}

	SplitPoint BVH::CalculateSplitpoint(const std::vector<PrimitiveIndex>& _range) const
	{
		std::vector<PrimitiveIndex> primitiveIndices = _range;

		SplitPoint splitPoint;
		splitPoint.SplitCost = std::numeric_limits<float>::infinity();

		int32_t splitDimension = GetSplitDimension(_range);
		if (splitDimension == -1)
		{
			return splitPoint;
		}
		std::sort(primitiveIndices.begin(), primitiveIndices.end(), [this, splitDimension](PrimitiveIndex first, PrimitiveIndex second)
		{
			return m_Primitives[first].V0.f[splitDimension] < m_Primitives[second].V0.f[splitDimension];
		});

		float leftBoundary = m_Primitives[primitiveIndices.front()].V0.f[splitDimension];
		// Sorted, so the width of this dimension is the difference between the first
		// and last element
		float splitWidth = m_Primitives[primitiveIndices.back()].V0.f[splitDimension] - 
			leftBoundary;

		auto splitPrimitive = primitiveIndices.cbegin();

		size_t Bins = 16u;
		// Don't iterate over the first and last bins, so that we're guaranteed to have at least one primitive
		// on both sides
		for (size_t bin = 1u; bin < Bins - 1; bin++)
		{
			// Place the separator at Bins number of locations to try as split points
			auto separator = (splitWidth / Bins) * bin + leftBoundary;

			// Find the first primitive past the separator, i.e. where the "bin ends"
			splitPrimitive = std::find_if(splitPrimitive, primitiveIndices.cend(), [separator, splitDimension, this](PrimitiveIndex index)
			{
				return m_Primitives[index].V0.f[splitDimension] >= separator;
			});

			auto costLeft = GetCost(primitiveIndices.cbegin(), splitPrimitive);
			auto costRight = GetCost(splitPrimitive, primitiveIndices.cend());
			auto totalCost = costLeft + costRight;
			if (totalCost < splitPoint.SplitCost)
			{
				splitPoint.Left = std::vector<PrimitiveIndex>(primitiveIndices.cbegin(), splitPrimitive);
				splitPoint.Right = std::vector<PrimitiveIndex>(splitPrimitive, primitiveIndices.cend());
				splitPoint.SplitCost = totalCost;
			}
		}
		return splitPoint;
	}

	float BVH::GetCost(std::vector<PrimitiveIndex>::const_iterator _start,
		std::vector<PrimitiveIndex>::const_iterator _end) const
	{
		return CalculateSmallestAABB(_start, _end).GetSurfaceArea()
			* std::distance(_start, _end);
	}

	int32_t BVH::GetSplitDimension(const std::vector<PrimitiveIndex>& _range) const
	{
		float3 min = float3::Infinity();
		float3 max = float3::NegativeInfinity();
		for (auto index : _range)
		{
			min = min.ComponentMin(m_Primitives[index].V0);
			max = max.ComponentMax(m_Primitives[index].V0);
		}
		float3 extents = (max - min).Abs();

		// Every dimension is near zero
		if (extents.MagnitudeSquared() < 0.0001f)
		{
			return -1;
		}
		if (extents.x > extents.y && extents.x > extents.z)
		{
			return 0;
		}
		if (extents.y > extents.z)
		{
			return 1;
		}
		return 2;
	}

	AABB BVH::CalculateSmallestAABB(std::vector<PrimitiveIndex>::const_iterator _start,
		std::vector<PrimitiveIndex>::const_iterator _end) const
	{
		auto it = _start;
		auto primitive = &m_Primitives[*it];
		AABB boundingBox{ float3::ComponentMin({ primitive->V0, primitive->V1, primitive->V2 }),
						  float3::ComponentMax({ primitive->V0, primitive->V1, primitive->V2 }) };
		it++;
		for (; it != _end; it++)
		{
			primitive = &m_Primitives[*it];
			boundingBox.Min = float3::ComponentMin({ boundingBox.Min, primitive->V0, primitive->V1, primitive->V2 });
			boundingBox.Max = float3::ComponentMax({ boundingBox.Max, primitive->V0, primitive->V1, primitive->V2 });
		}
		return boundingBox;
	}
}
