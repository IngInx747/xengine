#pragma once
#ifndef XE_MESH_H
#define XE_MESH_H

#include <vector>
#include <functional>

#include <vendor/glm/common.hpp>

#include <geometry/aabb.h>

namespace xengine
{
	class Mesh
	{
	public:
		Mesh();
		Mesh(const std::vector<glm::vec3>& positions, const std::vector<unsigned int>& indices);
		Mesh(const std::vector<glm::vec3>& positions, const std::vector<glm::vec2>& texCoords, const std::vector<unsigned int>& indices);
		Mesh(const std::vector<glm::vec3>& positions, const std::vector<glm::vec2>& texCoords, const std::vector<glm::vec3>& normals, const std::vector<unsigned int>& indices);
		Mesh(const std::vector<glm::vec3>& positions, const std::vector<glm::vec2>& texCoords, const std::vector<glm::vec3>& normals, const std::vector<glm::vec3>& tangents, const std::vector<glm::vec3>& bitangents, const std::vector<unsigned int>& indices);

		~Mesh();

		inline unsigned int VAO() const { return vao; }
		inline unsigned int IBO() const { return ibo; }
		inline unsigned int Topology() const { return topology; }
		inline unsigned int& Topology() { return topology; }

		// Commit all buffers and attributes to GPU for rendering
		void CommitGpuData(bool flag = true);

		// Delete all buffers stored in GPU
		void DeleteGpuData();

	protected:
		void commitDataInterleaved();
		void commitDataBatch();

	public:
		// mesh data
		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> texCoords;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec3> tangents;
		std::vector<glm::vec3> bitangents;

		// indices (optional)
		std::vector<unsigned int> indices;

		// bounding box
		AABB aabb;

	protected:
		// OpenGL stuff
		unsigned int vao = 0;
		unsigned int vbo = 0;
		unsigned int ibo = 0;
		unsigned int topology;
	};
}
#endif // !XE_MESH_H
