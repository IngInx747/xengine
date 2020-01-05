#include "mesh_manager.h"

#include <cstdio>
#include <cstdarg> // Variadic Function

#include <vendor/glad/glad.h>
#include <vendor/glm/glm.hpp>

#include <vendor/assimp/scene.h>
#include <vendor/assimp/Importer.hpp>
#include <vendor/assimp/postprocess.h>

#include <utility/log.h>

#include "primitive.h"

namespace xengine
{
	std::vector<std::shared_ptr<Mesh>> MeshManager::_meshes{};
	std::unordered_map<std::string, Mesh*> MeshManager::_meshTable{};

	std::vector<std::shared_ptr<Mesh>> MeshManager::_defaultMeshes{};
	std::unordered_map<std::string, Mesh*> MeshManager::_defaultMeshTable{};

	Mesh* MeshManager::_nullMesh = nullptr;

	void MeshManager::Initialize()
	{
		generateDefaultMesh();

		_nullMesh = _defaultMeshTable["cube"];
	}

	void MeshManager::Clear()
	{
		ClearLocal();
		ClearPrimitives();
	}

	void MeshManager::ClearLocal()
	{
		_meshTable.clear();
		_meshes.clear();
	}

	void MeshManager::ClearPrimitives()
	{
		_defaultMeshTable.clear();
		_defaultMeshes.clear();
	}

	Mesh * MeshManager::LoadPrimitive(std::string type, ...)
	{
		std::shared_ptr<Mesh> mesh = nullptr;

		// Note: Parameters of functions that correspond to ... are promoted before passing
		// to your variadic function. char and short are promoted to int, float is promoted
		// to double, etc.

		va_list args_list;
		unsigned int argc = 0;

		std::string name = type;

		if (type == "quad")
		{
			auto it = _defaultMeshTable.find(name);
			if (it != _defaultMeshTable.end()) return it->second;

			mesh = std::make_shared<Quad>();
		}
		else if (type == "cube")
		{
			auto it = _defaultMeshTable.find(name);
			if (it != _defaultMeshTable.end()) return it->second;

			mesh = std::make_shared<Cube>();
		}
		else if (type == "plane")
		{
			auto it = _defaultMeshTable.find(name);
			if (it != _defaultMeshTable.end()) return it->second;

			mesh = std::make_shared<Plane>();
		}
		else if (type == "sphere")
		{
			unsigned int xseg = 16, yseg = 8;

			va_start(args_list, type);
			xseg = va_arg(args_list, unsigned int);
			yseg = va_arg(args_list, unsigned int);
			va_end(args_list);

			name.append("_");
			name.append(std::to_string(xseg));
			name.append("_");
			name.append(std::to_string(yseg));

			auto it = _defaultMeshTable.find(name);
			if (it != _defaultMeshTable.end()) return it->second;

			mesh = std::make_shared<Sphere>(xseg, yseg);
		}
		else if (type == "torus")
		{
			unsigned int xseg = 16, yseg = 8;
			float r1 = 1.0f, r2 = 0.25f;

			va_start(args_list, type);
			xseg = va_arg(args_list, unsigned int);
			yseg = va_arg(args_list, unsigned int);
			r1 = static_cast<float>(va_arg(args_list, double));
			r2 = static_cast<float>(va_arg(args_list, double));
			va_end(args_list);

			name.append("_");
			name.append(std::to_string(xseg));
			name.append("_");
			name.append(std::to_string(yseg));
			name.append("_");
			name.append(std::to_string(r1));
			name.append("_");
			name.append(std::to_string(r2));

			auto it = _defaultMeshTable.find(name);
			if (it != _defaultMeshTable.end()) return it->second;

			mesh = std::make_shared<Torus>(xseg, yseg, r1, r2);
		}
		else if (false)
		{
			// TODO: more primitives
		}
		else
		{
			Log::Message("[MeshManager] Primitive \"" + type + "\" not supported", Log::ERROR);
			return _nullMesh;
		}

		if (!mesh)
		{
			Log::Message("[MeshManager] Primitive \"" + name + "\" loading failed", Log::ERROR);
			return _nullMesh;
		}

		Log::Message("[MeshManager] Mesh \"" + name + "\" loaded successfully", Log::INFO);
		_defaultMeshes.push_back(mesh);
		_defaultMeshTable[name] = mesh.get();

		return mesh.get();
	}

	Mesh * MeshManager::Get(const std::string & name)
	{
		{
			auto it = _meshTable.find(name);
			if (it != _meshTable.end()) return it->second;
		}

		{
			auto it = _defaultMeshTable.find(name);
			if (it != _defaultMeshTable.end()) return it->second;
		}

		Log::Message("[MeshManager] Material \"" + name + "\" not found", Log::WARN);
		return _nullMesh;
	}

	void MeshManager::generateDefaultMesh()
	{
		LoadPrimitive("quad");
		LoadPrimitive("cube");
		LoadPrimitive("sphere", 16, 8);
		LoadPrimitive("sphere", 256, 128);
	}

	////////////////////////////////////////////////////////////////
	// API
	////////////////////////////////////////////////////////////////

	Mesh * MeshManager::LoadFromModel(aiMesh * aMesh, glm::vec3 & vmin, glm::vec3 & vmax)
	{
		aiString aString = aMesh->mName;
		std::string name{ aString.C_Str() };

		// Note: Meshes can be named, but this is not a requirement and leaving
		// this field empty is totally fine. There are mainly three uses for mesh names:
		// @ some formats name nodes and meshes independently.
		// @ Vertex animations refer to meshes by their names.
		// @ importers tend to split meshes up to meet the one - material - per - mesh
		//   requirement.  Assigning the same(dummy) name to each of the result meshes aids
		//   the caller at recovering the original mesh partitioning.

		// Note: Sometime mesh(es) in a node is nameless. Assimp importer writes mesh name
		// the same as material name i.e., mesh name is NOT unique through out the model.

		//auto it = _meshTable.find(name);
		//if (it != _meshTable.end()) return it->second;

		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

		std::vector<glm::vec3>& positions = mesh->positions;
		std::vector<glm::vec2>& texCoords = mesh->texCoords;
		std::vector<glm::vec3>& normals = mesh->normals;
		std::vector<glm::vec3>& tangents = mesh->tangents;
		std::vector<glm::vec3>& bitangents = mesh->bitangents;
		std::vector<unsigned int>& indices = mesh->indices;

		positions.resize(aMesh->mNumVertices);
		normals.resize(aMesh->mNumVertices);
		indices.resize(aMesh->mNumFaces * 3);

		if (aMesh->mNumUVComponents > 0)
		{
			texCoords.resize(aMesh->mNumVertices);
			tangents.resize(aMesh->mNumVertices);
			bitangents.resize(aMesh->mNumVertices);
		}

		for (unsigned int i = 0; i < aMesh->mNumVertices; ++i)
		{
			positions[i] = glm::vec3(aMesh->mVertices[i].x, aMesh->mVertices[i].y, aMesh->mVertices[i].z);

			normals[i] = glm::vec3(aMesh->mNormals[i].x, aMesh->mNormals[i].y, aMesh->mNormals[i].z);

			if (aMesh->mTextureCoords[0])
				texCoords[i] = glm::vec2(aMesh->mTextureCoords[0][i].x, aMesh->mTextureCoords[0][i].y);

			if (aMesh->mTangents)
			{
				tangents[i] = glm::vec3(aMesh->mTangents[i].x, aMesh->mTangents[i].y, aMesh->mTangents[i].z);
				bitangents[i] = glm::vec3(aMesh->mBitangents[i].x, aMesh->mBitangents[i].y, aMesh->mBitangents[i].z);
			}

			vmin = glm::min(vmin, positions[i]);
			vmax = glm::max(vmax, positions[i]);
		}

		for (unsigned int i = 0; i < aMesh->mNumFaces; ++i)
		{
			indices[i * 3 + 0] = aMesh->mFaces[i].mIndices[0];
			indices[i * 3 + 1] = aMesh->mFaces[i].mIndices[1];
			indices[i * 3 + 2] = aMesh->mFaces[i].mIndices[2];
		}

		mesh->aabb.BuildFromVertices(positions);

		mesh->Topology() = GL_TRIANGLES;

		mesh->CommitGpuData();

		_meshes.push_back(mesh);
		//_meshTable[name] = mesh.get();

		Log::Message("[MeshManager] Mesh \"" + name + "\" loaded successfully", Log::INFO);

		return mesh.get();
	}
}