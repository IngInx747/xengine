#include "line_strip.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace xengine
{
	LineStrip::LineStrip(float width, unsigned int seg)
	{
		float dx = 1.0f / seg;

		for (unsigned int i = 0; i < seg; ++i)
		{
			float u = static_cast<float>(i) * dx;
			positions.push_back({ -0.5f + u,  0.5f * width, 0.0f });
			positions.push_back({ -0.5f + u, -0.5f * width, 0.0f });
			texCoords.push_back({ u,1.0f });
			texCoords.push_back({ u,0.0f });
			normals.push_back({ 0.0f, 0.0f, 1.0f });
			normals.push_back({ 0.0f, 0.0f, 1.0f });
		}

		aabb.BuildFromVertices(positions);

		topology = GL_TRIANGLE_STRIP;

		CommitData();
	}
}
