#include "mesh_loader.h"

#include <vendor/glad/glad.h>
#include <vendor/glm/glm.hpp>

#include <vendor/assimp/scene.h>
#include <vendor/assimp/Importer.hpp>
#include <vendor/assimp/postprocess.h>

#include <utility/log.h>

#include "primitive.h"

namespace xengine
{
	Mesh LoadMesh_Impl_Assimp(aiMesh * aMesh)
	{
		// Note: Meshes can be named, but this is not a requirement and leaving
		// this field empty is totally fine. There are mainly three uses for mesh names:
		// @ some formats name nodes and meshes independently.
		// @ Vertex animations refer to meshes by their names.
		// @ importers tend to split meshes up to meet the one - material - per - mesh
		//   requirement.  Assigning the same(dummy) name to each of the result meshes aids
		//   the caller at recovering the original mesh partitioning.

		// Note: Sometime mesh(es) in a node is nameless. Assimp importer writes mesh name
		// the same as material name i.e., mesh name is NOT unique through out the model.

		aiString aString = aMesh->mName;
		std::string name{ aString.C_Str() };

		Mesh mesh;

		std::vector<glm::vec3>& positions = mesh.Positions();
		std::vector<glm::vec2>& texCoords = mesh.TexCoords();
		std::vector<glm::vec3>& normals = mesh.Normals();
		std::vector<glm::vec3>& tangents = mesh.Tangents();
		std::vector<glm::vec3>& bitangents = mesh.Bitangents();
		std::vector<unsigned int>& indices = mesh.Indices();

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
		}

		for (unsigned int i = 0; i < aMesh->mNumFaces; ++i)
		{
			indices[i * 3 + 0] = aMesh->mFaces[i].mIndices[0];
			indices[i * 3 + 1] = aMesh->mFaces[i].mIndices[1];
			indices[i * 3 + 2] = aMesh->mFaces[i].mIndices[2];
		}

		mesh.Commit();
		mesh.Topology() = GL_TRIANGLES;

		Log::Message("[MeshLoader] Mesh \"" + name + "\" loaded successfully", Log::INFO);

		return mesh;
	}
}