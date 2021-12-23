#include "./scene/model_loading.h"
#include "./core/graphics/color3.h"
#include "./raytracing/bvh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace CRT
{
	void ModelLoading::LoadModel(Scene* _scene, Material* material, float3 _offset, const std::string& _filepath)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(_filepath, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

		std::vector<Triangle> triangles;
		for (size_t i = 0; i < scene->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[i];
			int numFaces = mesh->mNumFaces;
			triangles.reserve(numFaces);

			for (int j = 0; j < numFaces; j++)
			{
				const aiFace& face = mesh->mFaces[j];
				aiVector3D pos0 = mesh->mVertices[face.mIndices[0]];
				aiVector3D pos1 = mesh->mVertices[face.mIndices[1]];
				aiVector3D pos2 = mesh->mVertices[face.mIndices[2]];

				aiVector3D uv0;
				aiVector3D uv1;
				aiVector3D uv2;
				if (mesh->HasTextureCoords(0))
				{
					uv0 = mesh->mTextureCoords[0][face.mIndices[0]];
					uv1 = mesh->mTextureCoords[0][face.mIndices[1]];
					uv2 = mesh->mTextureCoords[0][face.mIndices[2]];
				}
				else
				{
					uv0 = aiVector3D(0.0f, 0.0f, 0.0f);
					uv1 = aiVector3D(0.0f, 0.0f, 0.0f);
					uv2 = aiVector3D(0.0f, 0.0f, 0.0f);
				}

				aiVector3D normal = mesh->HasNormals() ? mesh->mNormals[face.mIndices[0]] : aiVector3D(1.0f, 1.0f, 1.0f);

				triangles.emplace_back(
					float3(pos0.x, pos0.y, pos0.z) + _offset,
					float3(pos1.x, pos1.y, pos1.z) + _offset,
					float3(pos2.x, pos2.y, pos2.z) + _offset,
					float2(uv0.x, uv0.y),
					float2(uv1.x, uv1.y),
					float2(uv2.x, uv2.y),
					float3(normal.x, normal.y, normal.z));
			}
			for (const Triangle& triangle : triangles)
			{
				_scene->AddTriangle(triangle, material);
			}
			triangles.clear();
		}
	}
}
