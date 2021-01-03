#include "model_loader.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <geometry/constant.h>
#include <utility/log.h>
#include <mesh/mesh_loader.h>
#include <graphics/material_loader.h>

namespace xengine
{
	static Model * LoadModel_Impl_Assimp(aiNode * aNode, const aiScene * aScene, const std::string & path)
	{
		// Note: The name might be empty (length of zero) but all nodes which need
		// to be accessed afterwards by bones or anims are usually named. Multiple
		// nodes may have the same name, but nodes which are accessed by bones (see
		// aiBone and aiMesh::mBones) must be unique.

		aiString aString = aNode->mName;
		std::string name{ aString.C_Str() };
		Model* node = new Model;

		for (unsigned int i = 0; i < aNode->mNumMeshes; ++i)
		{
			aiMesh* aMesh = aScene->mMeshes[aNode->mMeshes[i]];
			aiMaterial* aMaterial = aScene->mMaterials[aMesh->mMaterialIndex];

			Mesh mesh = LoadMesh_Impl_Assimp(aMesh);
			Material material = LoadMaterial_Impl_Assimp(aMaterial, path, aMesh);

			node->meshes.push_back(mesh);
			node->materials.push_back(material);
			node->aabbLocal.UnionAABB(mesh.Aabb()); // update local bounding box
		}

		for (unsigned int i = 0; i < aNode->mNumChildren; ++i)
		{
			Model * child = LoadModel_Impl_Assimp(aNode->mChildren[i], aScene, path);
			node->InsertChild(child);
			// current node won't update bounding box based on children's box
		}

		Log::Message("[ModelLoader] Node \"" + name + "\" loaded successfully", Log::INFO);

		return node;
	}

	Model * LoadModel_Impl_Assimp(const std::string & path)
	{
		Assimp::Importer importer;
		const aiScene* aScene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

		if (!aScene || aScene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !aScene->mRootNode)
		{
			Log::Message("[ModelLoader] Model \"" + path + "\" failed to load", Log::ERROR);
			return nullptr;
		}

		std::string directory = path.substr(0, path.find_last_of("/"));
		Model * node = LoadModel_Impl_Assimp(aScene->mRootNode, aScene, directory);

		if (!node)
		{
			Log::Message("[ModelManager] Model \"" + path + "\" loading failed", Log::ERROR);
			return nullptr;
		}

		return node;
	}
}