#pragma once
#include <vector>
#include <memory>
#include <optional>

#include <./raytracing/ray.h>
#include <./raytracing/shapes/triangle.h>
#include <./raytracing/aabb.h>

namespace CRT
{
	using Primitive = Triangle;
	using PrimitiveIndex = uint32_t;
	class Texture;

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

	struct PrimitiveNode
	{
		AABB Bounds;
		float3 Centroid;
	};

	struct SplitPoint
	{
		std::vector<PrimitiveIndex> Left;
		std::vector<PrimitiveIndex> Right;
		float SplitCost = 0.0f;
	};

	struct TraversalResult
	{
		std::optional<Manifest> Manifest;
		float Depth = 0.0f;
	};

	class BVH
	{
	public:
		BVH(const std::vector<Primitive>& _primitives, const Texture* _heightMap);

		TraversalResult GetNearestIntersection(const Ray& ray) const;
		void GetNearestIntersection(const RayPacket& ray, TraversalResultPacket& _result) const;

		uint64_t GetNodeCount() const;
	private:
		TraversalResult TraverseNode(const Ray& ray, const BVHNode& parentNode) const;
		void TraverseNode(const RayPacket& ray, TraversalResultPacket& _result, const BVHNode& parentNode, int _firstActive)const;

		TraversalResult GetNearest(const Ray& _ray, const PrimitiveRange& range) const;

		void Construct();
		BVHNode SplitChild(BVHNode _node, const std::vector<PrimitiveIndex>& _range, const std::vector<PrimitiveNode>& _primitiveNodes, AABB _centroidBounds, size_t _currenDepth);
		SplitPoint CalculateSplitpoint(const std::vector<PrimitiveIndex>& _range) const;
		int32_t GetSplitDimension(const std::vector<PrimitiveIndex>& _range) const;
		float GetCost(std::vector<PrimitiveIndex>::const_iterator _start,
			std::vector<PrimitiveIndex>::const_iterator _end) const;
		AABB CalculateSmallestAABB(std::vector<PrimitiveIndex>::const_iterator _start,
			std::vector<PrimitiveIndex>::const_iterator _end) const;

		constexpr static uint32_t MaxBins = 16u;
		const Texture* m_Heightmap;
		std::vector<Primitive> m_Primitives;
		std::vector<uint32_t> m_PrimitiveIndices;
		std::vector<BVHNode> m_Nodes;
		BVHNode m_RootNode;
		uint64_t m_MaxDepth = 0;
	};
}
