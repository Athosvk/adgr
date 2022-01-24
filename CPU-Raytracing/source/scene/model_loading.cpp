#include "./scene/model_loading.h"
#include "./core/graphics/color3.h"
#include "./raytracing/bvh.h"
#include "./raytracing/shapes/mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>

namespace CRT
{
	void ModelLoading::LoadModel(Scene* _scene, Material* material, float3 _offset, const std::string& _filepath)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(_filepath, aiProcessPreset_TargetRealtime_Quality
			| aiProcess_FindDegenerates | aiProcess_FindInvalidData
			| aiProcess_FixInfacingNormals);

		std::string error = importer.GetErrorString();
		if (!error.empty())
		{
			std::cout << "Error while loading mesh: " << error << "\n";
		}
		std::vector<Triangle> triangles;
		for (size_t i = 0; i < scene->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[i];
			int numFaces = mesh->mNumFaces;
			triangles.reserve(triangles.size() + numFaces);

			if (!mesh->HasTextureCoords(0))
			{
				std::cout << "Mesh " << i << " of file " << _filepath << " has no texture coordinates\n";
			}

			for (int j = 0; j < numFaces; j++)
			{
				const aiFace& face = mesh->mFaces[j];
				// Mesh might still have "lines"
				if (face.mNumIndices < 3)
				{
					continue;
				}
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
				
				aiVector3D n1, n2, n3;
				if (mesh->HasNormals())
				{
					n1 = mesh->mNormals[face.mIndices[0]];
					n2 = mesh->mNormals[face.mIndices[1]];
					n3 = mesh->mNormals[face.mIndices[2]];
				}
				else
				{
					n1 = n2 = n3 = aiVector3D(0.0f, 0.0f, 0.0f);
				}

				triangles.emplace_back(
					float3(pos0.x, pos0.y, pos0.z) + _offset,
					float3(pos1.x, pos1.y, pos1.z) + _offset,
					float3(pos2.x, pos2.y, pos2.z) + _offset,
					float2(uv0.x, uv0.y),
					float2(uv1.x, uv1.y),
					float2(uv2.x, uv2.y),
					float3(n1.x, n1.y, n1.z),
					float3(n2.x, n2.y, n2.z),
					float3(n3.x, n3.y, n3.z)
					);
			}
		}
		_scene->AddMesh(Mesh(std::move(triangles), material));
	}
}
