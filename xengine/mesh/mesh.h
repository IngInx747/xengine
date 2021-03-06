#pragma once
#ifndef XE_MESH_H
#define XE_MESH_H

#include <vector>

#include <glm/common.hpp>

#include <geometry/aabb.h>
#include <utility/smart_handle.h>

namespace xengine
{
	class MeshMomory : public SharedMemory
	{
	public:
		void Generate(); // allocate memory on GPU
		void Destory(); // free memory on GPU

	public:
		// bounding box
		AABB aabb;

		// OpenGL stuff
		unsigned int vao = 0;
		unsigned int vbo = 0;
		unsigned int ibo = 0;
		unsigned int topology;

		// geometry info
		unsigned int numVertices = 0;
		unsigned int numIndices = 0;

		// temporary mesh data
		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> texCoords;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec3> tangents;
		std::vector<glm::vec3> bitangents;
		std::vector<unsigned int> indices;
	};

	class Mesh : public SharedHandle<MeshMomory>
	{
	public:
		// Commit data to all resources
		void Commit(bool flag = true);

		explicit operator bool() const { return m_ptr && m_ptr->vao; }

		inline unsigned int VAO() const { return m_ptr->vao; }
		inline unsigned int IBO() const { return m_ptr->ibo; }
		inline unsigned int NumVtx() const { return m_ptr->numVertices; }
		inline unsigned int NumIds() const { return m_ptr->numIndices; }
		inline unsigned int Topology() const { return m_ptr->topology; }
		inline const AABB & Aabb() const { return m_ptr->aabb; }

		inline unsigned int& Topology() { return m_ptr->topology; }
		inline AABB & Aabb() { return m_ptr->aabb; }

		std::vector<glm::vec3>& Positions();
		std::vector<glm::vec2>& TexCoords();
		std::vector<glm::vec3>& Normals();
		std::vector<glm::vec3>& Tangents();
		std::vector<glm::vec3>& Bitangents();
		std::vector<unsigned int>& Indices();

	protected:
		// commit vertices data to GPU in an interleaved way
		void commitOglVertexInter();

		// commit vertices data to GPU separately (in batch)
		void commitOglVertexBatch();
	};
}
#endif // !XE_MESH_H
