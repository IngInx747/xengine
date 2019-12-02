#pragma once
#ifndef XE_MATERIAL_MANAGER_H
#define XE_MATERIAL_MANAGER_H

#include "material.h"

struct aiMaterial;
struct aiScene;
struct aiMesh;

namespace xengine
{
	class MaterialManager
	{
	public:
		static void Initialize();
		static void Clear();
		static void ClearScene();
		static void ClearDefault();

		// load a anonymous material into scene-specific resources
		static Material* Load(Shader* shader);

		// load a named material into scene-specific resources
		static Material* Load(const std::string& name, Shader* shader);

		// search for a named material loaded previously (if not exist, search in default resources)
		static Material* Get(const std::string& name);

		// returns a deepcopy of other material
		static Material* CopyFromOther(Material* other);

		// returns a deepcopy of the named material loaded previously (if not exist, search in default resources)
		static Material* CopyFromOther(const std::string& name);

		// API for model manager to load mesh from model file (wave-front format)
		static Material* LoadFromModel(aiMaterial* aMaterial, const std::string& directory, aiMesh* aMesh);

	private:
		static std::shared_ptr<Material> loadMaterial(Shader* shader);

		static void generateDefaultMaterial();

	private:
		// resources containers
		static std::vector<std::shared_ptr<Material>> _materials;

		// lookup tables
		static std::unordered_map<std::string, Material*> _materialTable;

		// material templates
		static std::vector<std::shared_ptr<Material>> _defaultMaterials;

		// lookup tables
		static std::unordered_map<std::string, Material*> _defaultMaterialTable;
	};
}

#endif // !XE_MATERIAL_MANAGER_H
