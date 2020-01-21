#include "mesh.h"

#include <vendor/glad/glad.h>

namespace xengine
{
	////////////////////////////////////////////////////////////////
	// Mesh Unique Instance
	////////////////////////////////////////////////////////////////

	void MeshMomory::Generate()
	{
	}

	void MeshMomory::Destory()
	{
		if (vao)
		{
			glDeleteVertexArrays(1, &vao);
			vao = 0;
		}

		if (vbo)
		{
			glDeleteBuffers(1, &vbo);
			vbo = 0;
		}

		if (ibo)
		{
			glDeleteBuffers(1, &ibo);
			ibo = 0;
		}
	}

	////////////////////////////////////////////////////////////////
	// Mesh
	////////////////////////////////////////////////////////////////

	std::vector<glm::vec3>& Mesh::Positions() { allocateMemory(); return m_ptr->positions; }
	std::vector<glm::vec2>& Mesh::TexCoords() { allocateMemory(); return m_ptr->texCoords; }
	std::vector<glm::vec3>& Mesh::Normals() { allocateMemory(); return m_ptr->normals; }
	std::vector<glm::vec3>& Mesh::Tangents() { allocateMemory(); return m_ptr->tangents; }
	std::vector<glm::vec3>& Mesh::Bitangents() { allocateMemory(); return m_ptr->bitangents; }
	std::vector<unsigned int>& Mesh::Indices() { allocateMemory(); return m_ptr->indices; }

	void Mesh::Commit(bool flag)
	{
		generate();

		m_ptr->numVertices = static_cast<unsigned int>(m_ptr->positions.size());
		m_ptr->numIndices = static_cast<unsigned int>(m_ptr->indices.size());
		m_ptr->aabb.BuildFromVertices(m_ptr->positions);

		// process buffer data as interleaved or seperate when specified
		if (flag)
		{
			commitOglVertexInter();
		}
		else
		{
			commitOglVertexBatch();
		}

		m_ptr->positions.clear();
		m_ptr->texCoords.clear();
		m_ptr->normals.clear();
		m_ptr->tangents.clear();
		m_ptr->bitangents.clear();
		m_ptr->indices.clear();
	}

	void Mesh::commitOglVertexInter()
	{
		if (!m_ptr->vao)
		{
			glGenVertexArrays(1, &m_ptr->vao);
			glGenBuffers(1, &m_ptr->vbo);
		}

		std::vector<float> data;

		std::vector<glm::vec3>& positions  = m_ptr->positions;
		std::vector<glm::vec2>& texCoords  = m_ptr->texCoords;
		std::vector<glm::vec3>& normals    = m_ptr->normals;
		std::vector<glm::vec3>& tangents   = m_ptr->tangents;
		std::vector<glm::vec3>& bitangents = m_ptr->bitangents;
		std::vector<unsigned int>& indices = m_ptr->indices;

		for (size_t i = 0; i < positions.size(); ++i)
		{
			data.push_back(positions[i].x);
			data.push_back(positions[i].y);
			data.push_back(positions[i].z);

			if (texCoords.size() > 0)
			{
				data.push_back(texCoords[i].x);
				data.push_back(texCoords[i].y);
			}

			if (normals.size() > 0)
			{
				data.push_back(normals[i].x);
				data.push_back(normals[i].y);
				data.push_back(normals[i].z);
			}

			if (tangents.size() > 0)
			{
				data.push_back(tangents[i].x);
				data.push_back(tangents[i].y);
				data.push_back(tangents[i].z);
			}

			if (bitangents.size() > 0)
			{
				data.push_back(bitangents[i].x);
				data.push_back(bitangents[i].y);
				data.push_back(bitangents[i].z);
			}
		}

		glBindVertexArray(m_ptr->vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_ptr->vbo);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);

		if (indices.size() > 0)
		{
			glGenBuffers(1, &m_ptr->ibo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ptr->ibo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
		}

		GLsizei stride = 3 * sizeof(float);
		if (texCoords.size() > 0)  stride += 2 * sizeof(float);
		if (normals.size() > 0)    stride += 3 * sizeof(float);
		if (tangents.size() > 0)   stride += 3 * sizeof(float);
		if (bitangents.size() > 0) stride += 3 * sizeof(float);

		size_t offset = 0;
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);

		offset += 3 * sizeof(float);

		if (texCoords.size() > 0)
		{
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
			offset += 2 * sizeof(float);
		}

		if (normals.size() > 0)
		{
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
			offset += 3 * sizeof(float);
		}

		if (tangents.size() > 0)
		{
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
			offset += 3 * sizeof(float);
		}

		if (bitangents.size() > 0)
		{
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
			offset += 3 * sizeof(float);
		}

		glBindVertexArray(0);
	}

	void Mesh::commitOglVertexBatch()
	{
		if (!m_ptr->vao)
		{
			glGenVertexArrays(1, &m_ptr->vao);
			glGenBuffers(1, &m_ptr->vbo);
		}

		std::vector<float> data;

		std::vector<glm::vec3>& positions  = m_ptr->positions;
		std::vector<glm::vec2>& texCoords  = m_ptr->texCoords;
		std::vector<glm::vec3>& normals    = m_ptr->normals;
		std::vector<glm::vec3>& tangents   = m_ptr->tangents;
		std::vector<glm::vec3>& bitangents = m_ptr->bitangents;
		std::vector<unsigned int>& indices = m_ptr->indices;

		for (size_t i = 0; i < positions.size(); ++i)
		{
			data.push_back(positions[i].x);
			data.push_back(positions[i].y);
			data.push_back(positions[i].z);
		}

		for (size_t i = 0; i < texCoords.size(); ++i)
		{
			data.push_back(texCoords[i].x);
			data.push_back(texCoords[i].y);
		}

		for (size_t i = 0; i < normals.size(); ++i)
		{
			data.push_back(normals[i].x);
			data.push_back(normals[i].y);
			data.push_back(normals[i].z);
		}

		for (size_t i = 0; i < tangents.size(); ++i)
		{
			data.push_back(tangents[i].x);
			data.push_back(tangents[i].y);
			data.push_back(tangents[i].z);
		}

		for (size_t i = 0; i < bitangents.size(); ++i)
		{
			data.push_back(bitangents[i].x);
			data.push_back(bitangents[i].y);
			data.push_back(bitangents[i].z);
		}

		glBindVertexArray(m_ptr->vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_ptr->vbo);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);

		if (indices.size() > 0)
		{
			glGenBuffers(1, &m_ptr->ibo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ptr->ibo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
		}

		size_t offset = 0;
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)offset);

		offset += positions.size() * sizeof(float);

		if (texCoords.size() > 0)
		{
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)offset);
			offset += texCoords.size() * sizeof(float);
		}

		if (normals.size() > 0)
		{
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)offset);
			offset += normals.size() * sizeof(float);
		}

		if (tangents.size() > 0)
		{
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)offset);
			offset += tangents.size() * sizeof(float);
		}

		if (bitangents.size() > 0)
		{
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)offset);
			offset += bitangents.size() * sizeof(float);
		}

		glBindVertexArray(0);
	}
}