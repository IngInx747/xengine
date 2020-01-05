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
		static void Initialize();
		static void Clear();
		static void ClearLocal();
		static void ClearPrimitives();

		// load primitive
		static Mesh* LoadPrimitive(std::string type, ...); // load meshed primitive (zero or more params)

		// get named mesh
		static Mesh* Get(const std::string& name);

		// API for model manager to load mesh from model file (wave-front format)
		static Mesh* LoadFromModel(aiMesh* aMesh, glm::vec3& vmin, glm::vec3& vmax);

	private:
		// generate pre-defined mesh
		static void generateDefaultMesh();

	private:
		// resources container
		static std::vector<std::shared_ptr<Mesh>> _meshes;

		// lookup table
		static std::unordered_map<std::string, Mesh*> _meshTable;

		// primitives
		static std::vector<std::shared_ptr<Mesh>> _defaultMeshes;

		// primitives lookup table
		static std::unordered_map<std::string, Mesh*> _defaultMeshTable;

		// null protector (if a mesh fails to load, this mesh will be the output)
		static Mesh* _nullMesh;
	};
}

#endif // !XE_MESH_MANAGER_H
