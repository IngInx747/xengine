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
		// initialize shader manager (load default resources)
		static void Initialize();

		// clear resources
		static void Clear();

		// clear local resources (loaded with scene)
		static void ClearLocal();

		// clear global resources (default resources or shared by multiple scenes)
		static void ClearGlobal();

		// load a anonymous material into scene-specific resources
		static Material* Load(const Shader& shader);

		// load a named material into scene-specific resources
		static Material* Load(const std::string& name, const Shader& shader);

		// search for a named material loaded previously (if not exist, search in default resources)
		static Material* Get(const std::string& name);

		// returns a deepcopy of other material
		static Material* CopyFromOther(Material* other);

		// returns a deepcopy of the named material loaded previously (if not exist, search in default resources)
		static Material* CopyFromOther(const std::string& name);

		// API for model manager to load mesh from model file (wave-front format)
		static Material* LoadFromModel(aiMaterial* aMaterial, const std::string& directory, aiMesh* aMesh);

	private:
		static std::shared_ptr<Material> loadMaterial(const Shader& shader);

		static void generateDefaultMaterial();

	private:
		// resources containers
		static std::vector<std::shared_ptr<Material>> _materials;

		// lookup tables
		static std::unordered_map<std::string, Material*> _materialTable;

	private:
		// material templates
		static std::vector<std::shared_ptr<Material>> _defaultMaterials;

		// lookup tables
		static std::unordered_map<std::string, Material*> _defaultMaterialTable;
	};
}

#endif // !XE_MATERIAL_MANAGER_H
