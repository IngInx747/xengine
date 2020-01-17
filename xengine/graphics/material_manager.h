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

		// register material to resource
		static void LoadLocalMaterial(const Material& other, const std::string& name);
		static void LoadGlobalMaterial(const Material& other, const std::string& name);

		// search for a named material loaded previously (if not exist, search in default resources)
		static Material Get(const std::string& name);

	private:
		static void generateDefaultMaterial();

	private:
		// lookup table
		static std::unordered_map<std::string, Material> g_localTable;

		// primitives lookup table
		static std::unordered_map<std::string, Material> g_globalTable;
	};
}

#endif // !XE_MATERIAL_MANAGER_H
