#include "bvh.h"

#include <algorithm>
#include <memory>
#include <array>

namespace CRT
{
	BVH::BVH(const std::vector<Primitive>& _primitives, const Texture* _heightMap) :
		m_Primitives(_primitives),
		m_Heightmap(_heightMap)
	{
		if (_primitives.empty())
		{
			throw std::exception("No primitives provided");
		}
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
			node.Bounds = prim.GetDisplacedBounds(1.0f);
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
		m_RootNode = SplitChild(m_RootNode, indices, primNodes, centroidBounds, 1);
	}

	BVHNode BVH::SplitChild(BVHNode _node, const std::vector<PrimitiveIndex>& _range, const std::vector<PrimitiveNode>& _primitiveNodes, AABB _centroidBounds, size_t _currentDepth)
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
			if (splitWidth == 0.0f)
			{
				_node.First = uint32_t(m_PrimitiveIndices.size());
				m_PrimitiveIndices.insert(m_PrimitiveIndices.end(), _range.begin(), _range.end());
				_node.Count = uint32_t(_range.size());
				return _node;
			}
			
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
			for (uint32_t i = uint32_t(bins.size() - 1); i > 0; i--)
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
			
			constexpr auto TraversalCostFactor = .5f;
			const auto parentCost = _node.Bounds.GetSurfaceArea() * _range.size();
			std::vector<PrimitiveIndex> leftPrimitives;
			std::vector<PrimitiveIndex> rightPrimitives;

			// Increase the cost by a percentage to account for the cost of having another bounding box added
			if (minCost * (1 + TraversalCostFactor) < parentCost)
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
				left = SplitChild(left, leftPrimitives, _primitiveNodes, leftCentroidBounds, _currentDepth + 1);

				BVHNode right;
				right.Bounds = rightBounds;
				right = SplitChild(right, rightPrimitives,_primitiveNodes, rightCentroidBounds, _currentDepth + 1);
				_node.Left = uint32_t(m_Nodes.size());
				m_Nodes.emplace_back(left);
				m_Nodes.emplace_back(right);
				return _node;
			}
		}
		m_MaxDepth = std::max(_currentDepth, m_MaxDepth);
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
		TraversalResult result = TraverseNode(_ray, m_RootNode);
		result.Depth += 1.0f / m_MaxDepth;
		return result;
	}

	void BVH::GetNearestIntersection(const RayPacket& ray, TraversalResultPacket& _result) const
	{
		int first = 0;
		if (!m_RootNode.Bounds.IntersectPacket(ray, first))
			return;

		//for (int i = 0; i < 16 * 16; i++)
		//{
		//	TraversalResult t = GetNearestIntersection(Ray(ray.O, ray.D[i]));
		//	_result.T[i] = t.Manifest->T;
		//}
		//return;

		TraverseNode(ray, _result, m_RootNode, first);
	}

	uint64_t BVH::GetNodeCount() const
	{
		// Add one for the root node
		return m_Nodes.size() + 1;
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
			if (resultRight.Depth > result.Depth)
			{
				result.Depth = resultRight.Depth;
			}
		}
		result.Depth += 1.0f / m_MaxDepth;
		return result;
	}

	void BVH::TraverseNode(const RayPacket& ray, TraversalResultPacket& _result, const BVHNode& parentNode, int _firstActive) const
	{
		if (parentNode.Count > 0)
		{
			for (uint32_t i = 0; i < parentNode.Count; i++)
			{
				auto primitive = m_Primitives[m_PrimitiveIndices[i + parentNode.First]];
				primitive.Intersect(ray, _result, _firstActive, m_PrimitiveIndices[i + parentNode.First]);
			}

			//for (int i = _firstActive; i < 16 * 16; i++)
			//{
			//	TraversalResult t = GetNearest(Ray(ray.O, ray.D[i]), { parentNode.First, parentNode.Count });
			//	if (t.Manifest->T < 1000.0f && t.Manifest->T > 0.0f)
			//		_result.T[i] = t.Manifest->T;
			//}
			return;
		}

		int fl = _firstActive;
		const auto& leftNode = m_Nodes[parentNode.Left];
		if (leftNode.Bounds.IntersectPacket(ray, fl))
		{
			TraverseNode(ray, _result, leftNode, fl);
		}

		int fr = _firstActive;
		const auto& rightNode = m_Nodes[parentNode.Left + 1ull];
		if (rightNode.Bounds.IntersectPacket(ray, fr))
		{
			TraverseNode(ray, _result, rightNode, fr);
		}
	}

	TraversalResult BVH::GetNearest(const Ray& _ray, const PrimitiveRange& range) const
	{
		TraversalResult result;
		for (uint32_t i = 0; i < range.Count; i++)
		{
			auto primitive = m_Primitives[m_PrimitiveIndices[i + range.FirstPrimitiveIndex]];
			Manifest manifest;
			if (primitive.IntersectDisplaced(_ray, manifest, m_Heightmap)
				&& (!result.Manifest || manifest.T < result.Manifest->T))
			{
				result.Manifest = manifest;
			}
		}
		return result;
	}
}
