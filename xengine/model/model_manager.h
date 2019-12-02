#pragma once
#ifndef XE_MODEL_MANAGER_H
#define XE_MODEL_MANAGER_H

#include "model.h"

struct aiNode;
struct aiScene;

namespace xengine
{
	class ModelManager
	{
	public:
		static void Initialize();
		static void Clear();
		static void ClearScene();
		static void ClearDefault();

		// load model from wave-front file
		static Model* LoadFromObj(const std::string& name, const std::string& path, bool use_dft_mtr = true);

		// get named model
		static Model* Get(const std::string& name);

	private:
		// load model from wave-front file
		static Model* loadFromObj(aiNode* aNode, const aiScene* aScene, const std::string& directory, bool use_dft_mtr);

		static void generateDefaultModel();

	private:
		// resources containers
		static std::vector<std::shared_ptr<Model>> _models;

		// lookup tables
		static std::unordered_map<std::string, Model*> _modelTable;

		// prefabricated models
		static std::vector<std::shared_ptr<Model>> _defaultModels;

		// lookup tables
		static std::unordered_map<std::string, Model*> _defaultModelTable;
	};
}

#endif // !XE_MODEL_MANAGER_H
