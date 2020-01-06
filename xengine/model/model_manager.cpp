#include "model_manager.h"

#include <vendor/assimp/scene.h>
#include <vendor/assimp/Importer.hpp>
#include <vendor/assimp/postprocess.h>

#include <geometry/constant.h>
#include <utility/log.h>
#include <mesh/mesh_manager.h>
#include <graphics/material_manager.h>

namespace xengine
{
	std::vector<std::shared_ptr<Model>> ModelManager::_models{};
	std::unordered_map<std::string, Model*> ModelManager::_modelTable{};

	std::vector<std::shared_ptr<Model>> ModelManager::_defaultModels{};
	std::unordered_map<std::string, Model*> ModelManager::_defaultModelTable{};

	void ModelManager::Initialize()
	{
		generateDefaultModel();
	}

	void ModelManager::Clear()
	{
		ClearLocal();
		ClearGlobal();
	}

	void ModelManager::ClearLocal()
	{
		_modelTable.clear();
		_models.clear();
	}

	void ModelManager::ClearGlobal()
	{
		_defaultModelTable.clear();
		_defaultModels.clear();
	}

	Model * ModelManager::LoadFromObj(const std::string & name, const std::string & path, bool use_dft_mtr)
	{
		auto it = _modelTable.find(name);
		if (it != _modelTable.end()) return it->second;

		Log::Message("[ModelManager] Loading model \"" + name + "\" from \"" + path + "\" ...", Log::INFO);

		Assimp::Importer importer;
		const aiScene* aScene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

		if (!aScene || aScene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !aScene->mRootNode)
		{
			Log::Message("[ModelManager] Model \"" + path + "\" failed to load", Log::ERROR);
			return nullptr;
		}

		std::string directory = path.substr(0, path.find_last_of("/"));
		Model * node = loadFromObj(aScene->mRootNode, aScene, directory, use_dft_mtr);

		if (!node)
		{
			Log::Message("[ModelManager] Model \"" + name + "\" loading failed", Log::ERROR);
			return nullptr;
		}

		_modelTable[name] = node; // only added root node to model lookup table

		Log::Message("[ModelManager] Model \"" + name + "\" loaded successfully", Log::INFO);

		return node;
	}

	Model * ModelManager::Get(const std::string & name)
	{
		auto it = _modelTable.find(name);
		if (it != _modelTable.end()) return it->second;

		Log::Message("[ModelManager] Model \"" + name + "\" not found", Log::WARN);
		return nullptr;
	}

	Model * ModelManager::loadFromObj(aiNode * aNode, const aiScene * aScene, const std::string & directory, bool use_dft_mtr)
	{
		aiString aString = aNode->mName;
		std::string name{ aString.C_Str() };

		// Note: The name might be empty (length of zero) but all nodes which need
		// to be accessed afterwards by bones or anims are usually named. Multiple
		// nodes may have the same name, but nodes which are accessed by bones (see
		// aiBone and aiMesh::mBones) must be unique.
		//if (name.size() > 0)
		//{
		//	auto it = _modelTable.find(name);
		//	if (it != _modelTable.end()) return it->second;
		//}

		std::shared_ptr<Model> node = std::make_shared<Model>();

		for (unsigned int i = 0; i < aNode->mNumMeshes; ++i)
		{
			glm::vec3 vmin{ kInf }, vmax{ -kInf };

			aiMesh* aMesh = aScene->mMeshes[aNode->mMeshes[i]];
			aiMaterial* aMaterial = aScene->mMaterials[aMesh->mMaterialIndex];

			Mesh* mesh = MeshManager::LoadFromModel(aMesh, vmin, vmax);
			Material* material = nullptr;

			if (use_dft_mtr) material = MaterialManager::LoadFromModel(aMaterial, directory, aMesh);

			node->meshes.push_back(mesh);
			node->materials.push_back(material);
			node->aabbLocal.UnionAABB(mesh->Aabb()); // update local bounding box
		}

		for (unsigned int i = 0; i < aNode->mNumChildren; ++i)
		{
			Model * child = loadFromObj(aNode->mChildren[i], aScene, directory, use_dft_mtr);
			node->InsertChild(child);

			// current node won't update bounding box based on children's box
		}

		_models.push_back(node); // Resource container registering
		//_modelTable[name] = node.get(); // inner nodes won't be registered to model lookup table

		Log::Message("[ModelManager] Node \"" + name + "\" loaded successfully", Log::INFO);

		return node.get();
	}

	void ModelManager::generateDefaultModel()
	{
		// TODO: add a utal teapot
	}
}
