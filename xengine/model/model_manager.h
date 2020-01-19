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
		// initialize shader manager (load default resources)
		static void Initialize();

		// clear resources
		static void Clear();

		// clear local resources (loaded with scene)
		static void ClearLocal();

		// clear global resources (default resources or shared by multiple scenes)
		static void ClearGlobal();

		// register a named model into resource (for sake of resource recycling)
		static void RegisterLocalModel(const std::string & name, Model* model);

		// register a named model into global resource
		static void RegisterGlobalModel(const std::string & name, Model* model);

		// load model from file
		static Model* LoadLocalModel(const std::string& name, const std::string& path);
		static Model* LoadGlobalModel(const std::string& name, const std::string& path);

		// get named model
		static Model* Get(const std::string& name);

	private:
		static Model* loadModel(const std::string& name, const std::string& path, std::unordered_map<std::string, Model*>& table);

		static void generateDefaultModel();

	private:
		// lookup tables
		static std::unordered_map<std::string, Model*> g_localTable;

		// lookup tables
		static std::unordered_map<std::string, Model*> g_globalTable;
	};
}

#endif // !XE_MODEL_MANAGER_H
