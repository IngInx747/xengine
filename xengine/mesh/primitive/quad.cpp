#include "quad.h"

#include <vendor/glad/glad.h>
#include <vendor/glm/glm.hpp>

namespace xengine
{
	Quad::Quad()
	{
		positions =
		{
			{ -1.0f, -1.0f,  0.0f, },
			{  1.0f, -1.0f,  0.0f, },
			{  1.0f,  1.0f,  0.0f, },
			{ -1.0f,  1.0f,  0.0f, },
		};

		texCoords = {
			{  0.0f,  0.0f, },
			{  1.0f,  0.0f, },
			{  1.0f,  1.0f, },
			{  0.0f,  1.0f, },
		};

		indices = {
			0,   1,   2,
			2,   3,   0
		};

		topology = GL_TRIANGLES;

		CommitGpuData();
	}

	Quad::Quad(float width, float height)
	{
		positions =
		{
			{ -width, -height,  0.0f, },
			{  width, -height,  0.0f, },
			{  width,  height,  0.0f, },
			{ -width,  height,  0.0f, },
		};

		texCoords = {
			{  0.0f,  0.0f, },
			{  1.0f,  0.0f, },
			{  1.0f,  1.0f, },
			{  0.0f,  1.0f, },
		};

		indices = {
			0,   1,   2,
			2,   3,   0
		};

		topology = GL_TRIANGLES;

		CommitGpuData();
	}
}