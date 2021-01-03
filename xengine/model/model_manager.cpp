#include "model_manager.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <utility/log.h>

#include "model_loader.h"

namespace xengine
{
	std::unordered_map<std::string, Model*> ModelManager::g_localTable{};
	std::unordered_map<std::string, Model*> ModelManager::g_globalTable{};

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
		for (auto it = g_localTable.begin(); it != g_localTable.end(); ++it)
			delete it->second;

		g_localTable.clear();
	}

	void ModelManager::ClearGlobal()
	{
		for (auto it = g_globalTable.begin(); it != g_globalTable.end(); ++it)
			delete it->second;

		g_globalTable.clear();
	}

	void ModelManager::RegisterLocalModel(const std::string & name, Model * model)
	{
		g_localTable[name] = model;
	}

	void ModelManager::RegisterGlobalModel(const std::string & name, Model * model)
	{
		g_globalTable[name] = model;
	}

	Model * ModelManager::LoadLocalModel(const std::string & name, const std::string & path)
	{
		return loadModel(name, path, g_localTable);
	}

	Model * ModelManager::LoadGlobalModel(const std::string & name, const std::string & path)
	{
		return loadModel(name, path, g_globalTable);
	}

	Model * ModelManager::Get(const std::string & name)
	{
		{
			auto it = g_localTable.find(name);
			if (it != g_localTable.end()) return it->second;
		}

		{
			auto it = g_globalTable.find(name);
			if (it != g_globalTable.end()) return it->second;
		}

		Log::Message("[ModelManager] Model \"" + name + "\" not found", Log::WARN);
		return nullptr;
	}

	Model * ModelManager::loadModel(const std::string & name, const std::string & path, std::unordered_map<std::string, Model*>& table)
	{
		auto it = table.find(name);
		if (it != table.end()) return it->second;

		Log::Message("[ModelManager] Loading model \"" + name + "\" from \"" + path + "\" ...", Log::INFO);

		Model * node = LoadModel_Impl_Assimp(path);

		g_localTable[name] = node; // only added root node to model lookup table

		Log::Message("[ModelManager] Model \"" + name + "\" loaded successfully", Log::INFO);

		return node;
	}

	void ModelManager::generateDefaultModel()
	{
		// TODO: add a utal teapot
	}
}
