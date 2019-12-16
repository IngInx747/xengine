#include "plane.h"

#include <vendor/glad/glad.h>
#include <vendor/glm/glm.hpp>

namespace xengine
{
	Plane::Plane()
	{
		positions = {
			{ -0.5,  0.0, -0.5 },
			{ -0.5,  0.0,  0.5 },
			{  0.5,  0.0,  0.5 },
			{  0.5,  0.0, -0.5 },
		};

		texCoords = {
			{  0.0f,  0.0f },
			{  1.0f,  0.0f },
			{  1.0f,  1.0f },
			{  0.0f,  1.0f },
		};

		normals = {
			{  0.0,  1.0,  0.0 },
			{  0.0,  1.0,  0.0 },
			{  0.0,  1.0,  0.0 },
			{  0.0,  1.0,  0.0 },
		};

		tangents = {
			{  1.0,  0.0,  0.0 },
			{  1.0,  0.0,  0.0 },
			{  1.0,  0.0,  0.0 },
			{  1.0,  0.0,  0.0 },
		};

		bitangents = {
			{  0.0,  0.0,  1.0 },
			{  0.0,  0.0,  1.0 },
			{  0.0,  0.0,  1.0 },
			{  0.0,  0.0,  1.0 },
		};

		indices = {
			0,   1,   2,
			2,   3,   0
		};

		aabb.BuildFromVertices(positions);

		topology = GL_TRIANGLES;

		CommitGpuData();
	}
}