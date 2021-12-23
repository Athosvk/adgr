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
		std::vector<PrimitiveNode> primNodes;
		primNodes.reserve(m_Primitives.size());

		AABB triangleBounds = AABB::NegativeBox();
		AABB centroidBounds = AABB::NegativeBox();
		for (const Primitive& prim : m_Primitives)
		{
			PrimitiveNode node;
			node.Bounds.Min = float3::ComponentMin({ prim.V0, prim.V1, prim.V2 });
			node.Bounds.Max = float3::ComponentMax({ prim.V0, prim.V1, prim.V2 });
			node.Centroid = prim.GetCentroid();
			triangleBounds = triangleBounds.Extend(node.Bounds);
			centroidBounds = centroidBounds.Extend(node.Centroid);
			primNodes.emplace_back(node);
		}

		std::vector<PrimitiveIndex> indices;
		indices.reserve(m_Primitives.size());
		// -2 because we store the root node separately, while max nodes is bounded by 2n - 1
		m_Nodes.reserve(m_Primitives.size() * 2 - 2);
		for (uint32_t i = 0u; i < uint32_t(m_Primitives.size()); i++)
		{
			indices.emplace_back(i);
		}
		m_RootNode.Bounds = triangleBounds;
		m_RootNode = SplitChild(m_RootNode, indices, primNodes, centroidBounds);
	}

	BVHNode BVH::SplitChild(BVHNode _node, const std::vector<PrimitiveIndex>& _range, const std::vector<PrimitiveNode>& _primitiveNodes, AABB _centroidBounds)
	{
		if (_range.size() > 1)
		{
			int splitDimension;
			float3 centroidDimensions = _centroidBounds.GetDimensions();
			if (centroidDimensions.MagnitudeSquared() < 0.0001f)
			{
				splitDimension = -1;
			}
			if (centroidDimensions.x > centroidDimensions.y && centroidDimensions.x > centroidDimensions.z)
			{
				splitDimension = 0;
			}
			else if (centroidDimensions.y > centroidDimensions.z)
			{
				splitDimension = 1;
			}
			else
			{
				splitDimension = 2;
			}
			float splitWidth = centroidDimensions.f[splitDimension];
			
			struct Bin
			{
				AABB Bounds = AABB::NegativeBox();
				uint32_t PrimitiveCount = 0;
			};
			std::vector<Bin> bins(MaxBins);
			for (auto index : _range)
			{
				const auto& node = _primitiveNodes[index];
				float relativePosition = (node.Centroid.f[splitDimension] - _centroidBounds.Min.f[splitDimension]) / splitWidth;
				// Subtract some epsilon so that triangles always end up on the left side of the bin
				int bin = int(MaxBins * (1 - 0.000001f) * relativePosition);
				
				bins[bin].Bounds = bins[bin].Bounds.Extend(node.Bounds);
				bins[bin].PrimitiveCount++;
			}

			std::vector<float> leftPartitionCosts;
			int totalPrims = 0;
			AABB totalBounds = AABB::NegativeBox();
			for (int i = 0; i < bins.size() - 1; i++)
			{
				totalBounds = totalBounds.Extend(bins[i].Bounds);
				totalPrims += bins[i].PrimitiveCount;
				leftPartitionCosts.emplace_back(totalBounds.GetSurfaceArea() * totalPrims);
			}

			float minCost = std::numeric_limits<float>::infinity();
			uint32_t bestBin = 0;
			totalPrims = 0;
			totalBounds = AABB::NegativeBox();
			std::vector<float> rightPartitionCosts;
			for (uint32_t i = bins.size() - 1; i > 0; i--)
			{
				totalBounds = totalBounds.Extend(bins[i].Bounds);
				totalPrims += bins[i].PrimitiveCount;
				rightPartitionCosts.emplace_back(totalBounds.GetSurfaceArea()* totalPrims);
				auto cost = totalBounds.GetSurfaceArea() * totalPrims + leftPartitionCosts[i - 1];
				if (cost < minCost)
				{
					minCost = cost;
					bestBin = uint32_t(i);
				}
			}
			
			constexpr auto TraversalCost = 10.0f;
			const auto parentCost = _node.Bounds.GetSurfaceArea() * _range.size();
			std::vector<PrimitiveIndex> leftPrimitives;
			std::vector<PrimitiveIndex> rightPrimitives;
			if (minCost + TraversalCost < parentCost)
			{
				AABB leftBounds = AABB::NegativeBox();
				AABB rightBounds = AABB::NegativeBox();
				AABB leftCentroidBounds = AABB::NegativeBox();
				AABB rightCentroidBounds = AABB::NegativeBox();
				for (auto index : _range)
				{
					const auto& node = _primitiveNodes[index];
					float relativePosition = (node.Centroid.f[splitDimension] - _centroidBounds.Min.f[splitDimension]) / splitWidth;
					uint32_t bin = uint32_t(MaxBins * (1 - 0.000001f) * relativePosition);
					if (bin < bestBin)
					{
						leftBounds = leftBounds.Extend(node.Bounds);
						leftCentroidBounds = leftCentroidBounds.Extend(node.Centroid);
						leftPrimitives.emplace_back(index);
					}
					else
					{
						rightBounds = rightBounds.Extend(node.Bounds);
						rightCentroidBounds = rightCentroidBounds.Extend(node.Centroid);
						rightPrimitives.emplace_back(index);
					}
				}
				
				BVHNode left;
				left.Bounds = leftBounds;
				left = SplitChild(left, leftPrimitives, _primitiveNodes, leftCentroidBounds);

				BVHNode right;
				right.Bounds = rightBounds;
				right = SplitChild(right, rightPrimitives,_primitiveNodes, rightCentroidBounds);
				_node.Left = uint32_t(m_Nodes.size());
				m_Nodes.emplace_back(left);
				m_Nodes.emplace_back(right);
				return _node;
			}
		}
		_node.First = uint32_t(m_PrimitiveIndices.size());
		m_PrimitiveIndices.insert(m_PrimitiveIndices.end(), _range.begin(), _range.end());
		_node.Count = uint32_t(_range.size());
		return _node;
	}

	TraversalResult BVH::GetNearestIntersection(const Ray& _ray) const
	{
		if (!m_RootNode.Bounds.Intersects(_ray))
		{
			return {};
		}
		return TraverseNode(_ray, m_RootNode);
	}

	uint64_t BVH::GetNodeCount() const
	{
		return m_Nodes.size();
	}

	TraversalResult BVH::TraverseNode(const Ray& _ray, const BVHNode& _parentNode) const
	{
		if (_parentNode.Count > 0)
		{
			return GetNearest(_ray, { _parentNode.First, _parentNode.Count });
		}
		TraversalResult result;
		const auto& leftNode = m_Nodes[_parentNode.Left];
		if (leftNode.Bounds.Intersects(_ray))
		{
			auto resultLeft = TraverseNode(_ray, leftNode);
			result = resultLeft;
		}
		const auto& rightNode = m_Nodes[_parentNode.Left + 1ull];
		if (rightNode.Bounds.Intersects(_ray))
		{
			auto resultRight = TraverseNode(_ray, rightNode);
			if (resultRight.Manifest)
			{
				if (!result.Manifest || resultRight.Manifest->T < result.Manifest->T)
				{
					result = resultRight;
				}
			}
		}
		result.Traversals++;
		return result;
	}

	TraversalResult BVH::GetNearest(const Ray& _ray, const PrimitiveRange& range) const
	{
		TraversalResult result;
		for (uint32_t i = 0; i < range.Count; i++)
		{
			auto primitive = m_Primitives[m_PrimitiveIndices[i + range.FirstPrimitiveIndex]];
			Manifest manifest;
			if (primitive.Intersect(_ray, manifest) && (!result.Manifest || manifest.T < result.Manifest->T))
			{
				result.Manifest = manifest;
				result.Index = i + range.FirstPrimitiveIndex;
			}
		}
		return result;
	}
}
