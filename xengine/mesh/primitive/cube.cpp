#include "cube.h"

#include <vendor/glad/glad.h>
#include <vendor/glm/glm.hpp>

namespace xengine
{
	Cube::Cube()
	{
		positions = {
			// front
			{ -0.5, -0.5,  0.5 },
			{  0.5, -0.5,  0.5 },
			{  0.5,  0.5,  0.5 },
			{ -0.5,  0.5,  0.5 },
			// back
			{ -0.5, -0.5, -0.5 },
			{ -0.5,  0.5, -0.5 },
			{  0.5,  0.5, -0.5 },
			{  0.5, -0.5, -0.5 },
			// left
			{ -0.5, -0.5, -0.5 },
			{ -0.5, -0.5,  0.5 },
			{ -0.5,  0.5,  0.5 },
			{ -0.5,  0.5, -0.5 },
			// right
			{  0.5, -0.5,  0.5 },
			{  0.5, -0.5, -0.5 },
			{  0.5,  0.5, -0.5 },
			{  0.5,  0.5,  0.5 },
			// top
			{ -0.5,  0.5,  0.5 },
			{  0.5,  0.5,  0.5 },
			{  0.5,  0.5, -0.5 },
			{ -0.5,  0.5, -0.5 },
			// buttom
			{  0.5, -0.5, -0.5 },
			{  0.5, -0.5,  0.5 },
			{ -0.5, -0.5,  0.5 },
			{ -0.5, -0.5, -0.5 },
		};

		texCoords = {
			// front
			{  0.0f,  0.0f },
			{  1.0f,  0.0f },
			{  1.0f,  1.0f },
			{  0.0f,  1.0f },
			// back
			{  0.0f,  0.0f },
			{  0.0f,  1.0f },
			{  1.0f,  1.0f },
			{  1.0f,  0.0f },
			// left
			{  0.0f,  0.0f },
			{  1.0f,  0.0f },
			{  1.0f,  1.0f },
			{  0.0f,  1.0f },
			// right
			{  1.0f,  0.0f },
			{  0.0f,  0.0f },
			{  0.0f,  1.0f },
			{  1.0f,  1.0f },
			// top
			{  0.0f,  1.0f },
			{  1.0f,  1.0f },
			{  1.0f,  0.0f },
			{  0.0f,  0.0f },
			// buttom
			{  1.0f,  0.0f },
			{  1.0f,  1.0f },
			{  0.0f,  1.0f },
			{  0.0f,  0.0f },
		};

		normals = {
			// front
			{  0.0,  0.0,  1.0 },
			{  0.0,  0.0,  1.0 },
			{  0.0,  0.0,  1.0 },
			{  0.0,  0.0,  1.0 },
			// back
			{  0.0,  0.0, -1.0 },
			{  0.0,  0.0, -1.0 },
			{  0.0,  0.0, -1.0 },
			{  0.0,  0.0, -1.0 },
			// left
			{ -1.0,  0.0,  0.0 },
			{ -1.0,  0.0,  0.0 },
			{ -1.0,  0.0,  0.0 },
			{ -1.0,  0.0,  0.0 },
			// right
			{  1.0,  0.0,  0.0 },
			{  1.0,  0.0,  0.0 },
			{  1.0,  0.0,  0.0 },
			{  1.0,  0.0,  0.0 },
			// top
			{  0.0,  1.0,  0.0 },
			{  0.0,  1.0,  0.0 },
			{  0.0,  1.0,  0.0 },
			{  0.0,  1.0,  0.0 },
			// buttom
			{  0.0, -1.0,  0.0 },
			{  0.0, -1.0,  0.0 },
			{  0.0, -1.0,  0.0 },
			{  0.0, -1.0,  0.0 },
		};

		tangents = {
			// front
			{  1.0,  0.0,  0.0 },
			{  1.0,  0.0,  0.0 },
			{  1.0,  0.0,  0.0 },
			{  1.0,  0.0,  0.0 },
			// back
			{  1.0,  0.0,  0.0 },
			{  1.0,  0.0,  0.0 },
			{  1.0,  0.0,  0.0 },
			{  1.0,  0.0,  0.0 },
			// left
			{  0.0,  0.0,  1.0 },
			{  0.0,  0.0,  1.0 },
			{  0.0,  0.0,  1.0 },
			{  0.0,  0.0,  1.0 },
			// right
			{  0.0,  0.0,  1.0 },
			{  0.0,  0.0,  1.0 },
			{  0.0,  0.0,  1.0 },
			{  0.0,  0.0,  1.0 },
			// top
			{  1.0,  0.0,  0.0 },
			{  1.0,  0.0,  0.0 },
			{  1.0,  0.0,  0.0 },
			{  1.0,  0.0,  0.0 },
			// buttom
			{  1.0,  0.0,  0.0 },
			{  1.0,  0.0,  0.0 },
			{  1.0,  0.0,  0.0 },
			{  1.0,  0.0,  0.0 },
		};

		bitangents = {
			// front
			{  0.0,  1.0,  0.0 },
			{  0.0,  1.0,  0.0 },
			{  0.0,  1.0,  0.0 },
			{  0.0,  1.0,  0.0 },
			// back
			{  0.0,  1.0,  0.0 },
			{  0.0,  1.0,  0.0 },
			{  0.0,  1.0,  0.0 },
			{  0.0,  1.0,  0.0 },
			// left
			{  0.0,  1.0,  0.0 },
			{  0.0,  1.0,  0.0 },
			{  0.0,  1.0,  0.0 },
			{  0.0,  1.0,  0.0 },
			// right
			{  0.0,  1.0,  0.0 },
			{  0.0,  1.0,  0.0 },
			{  0.0,  1.0,  0.0 },
			{  0.0,  1.0,  0.0 },
			// top
			{  0.0,  0.0,  1.0 },
			{  0.0,  0.0,  1.0 },
			{  0.0,  0.0,  1.0 },
			{  0.0,  0.0,  1.0 },
			// buttom
			{  0.0,  0.0,  1.0 },
			{  0.0,  0.0,  1.0 },
			{  0.0,  0.0,  1.0 },
			{  0.0,  0.0,  1.0 },
		};

		indices = {
			// front
			0,   1,   2,
			2,   3,   0,
			// back
			4,   5,   6,
			6,   7,   4,
			// left
			8,   9,   10,
			10,  11,  8,
			// right
			12,  13,  14,
			14,  15,  12,
			// top
			16,  17,  18,
			18,  19,  16,
			// buttom
			20,  21,  22,
			22,  23,  20,
		};

		aabb.BuildFromVertices(positions);

		topology = GL_TRIANGLES;

		CommitData();
	}
}
