#pragma once
#ifndef XE_MESH_MANAGER_H
#define XE_MESH_MANAGER_H

#include <memory>
#include <unordered_map>

#include "mesh.h"

struct aiMesh;
struct aiScene;

namespace xengine
{
	class MeshManager
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

		// load primitive
		static Mesh LoadLocalPrimitive(std::string type, ...);
		static Mesh LoadGlobalPrimitive(std::string type, ...);

		// get named mesh
		static Mesh Get(const std::string& name);

	private:
		// load primitive
		static Mesh loadPrimitive(
			std::unordered_map<std::string, Mesh>& table,
			std::string type,
			va_list args_list); // load meshed primitive (zero or more params)

		// generate pre-defined mesh
		static void generateDefaultMesh();

	private:
		// lookup table
		static std::unordered_map<std::string, Mesh> g_localTable;

		// primitives lookup table
		static std::unordered_map<std::string, Mesh> g_globalTable;

		// null protector (if a mesh fails to load, this mesh will be the output)
		static Mesh _nullMesh;
	};
}

#endif // !XE_MESH_MANAGER_H
