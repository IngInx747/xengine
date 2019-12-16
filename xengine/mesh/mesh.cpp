#include "mesh.h"

#include <vendor/glad/glad.h>

namespace xengine
{
	Mesh::Mesh()
	{}

	Mesh::Mesh(
		const std::vector<glm::vec3>& positions,
		const std::vector<unsigned int>& indices)
		:
		positions(positions),
		indices(indices),
		aabb(positions)
	{}

	Mesh::Mesh(
		const std::vector<glm::vec3>& positions,
		const std::vector<glm::vec2>& texCoords,
		const std::vector<unsigned int>& indices)
		:
		positions(positions),
		texCoords(texCoords),
		indices(indices),
		aabb(positions)
	{}

	Mesh::Mesh(
		const std::vector<glm::vec3>& positions,
		const std::vector<glm::vec2>& texCoords,
		const std::vector<glm::vec3>& normals,
		const std::vector<unsigned int>& indices)
		:
		positions(positions),
		texCoords(texCoords),
		normals(normals),
		indices(indices),
		aabb(positions)
	{}

	Mesh::Mesh(
		const std::vector<glm::vec3>& positions,
		const std::vector<glm::vec2>& texCoords,
		const std::vector<glm::vec3>& normals,
		const std::vector<glm::vec3>& tangents,
		const std::vector<glm::vec3>& bitangents,
		const std::vector<unsigned int>& indices)
		:
		positions(positions),
		texCoords(texCoords),
		normals(normals),
		tangents(tangents),
		bitangents(bitangents),
		indices(indices),
		aabb(positions)
	{}

	Mesh::~Mesh()
	{
		DeleteGpuData();
	}

	void Mesh::CommitGpuData(bool flag)
	{
		// process buffer data as interleaved or seperate when specified
		if (flag)
		{
			commitDataInterleaved();
		}
		else
		{
			commitDataBatch();
		}

		texCoords.clear();
		normals.clear();
		tangents.clear();
		bitangents.clear();
	}

	void Mesh::commitDataInterleaved()
	{
		if (!vao)
		{
			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);
			glGenBuffers(1, &ibo);
		}

		std::vector<float> data;

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

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);

		if (indices.size() > 0)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
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

	void Mesh::commitDataBatch()
	{
		if (!vao)
		{
			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);
			glGenBuffers(1, &ibo);
		}

		std::vector<float> data;

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

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);

		if (indices.size() > 0)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
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

	void Mesh::DeleteGpuData()
	{
		if (vao)
		{
			glDeleteVertexArrays(1, &vao);
			vao = 0;
		}

		if(vbo)
		{
			glDeleteBuffers(1, &vbo);
			vbo = 0;
		}

		if(ibo)
		{
			glDeleteBuffers(1, &ibo);
			ibo = 0;
		}
	}
}