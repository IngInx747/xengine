#include "primitive.h"

#include <vendor/glad/glad.h>
#include <vendor/glm/glm.hpp>

#include <geometry/constant.h>

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

		Commit();
		m_ptr->topology = GL_TRIANGLES;
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

		Commit();
		m_ptr->topology = GL_TRIANGLES;
	}

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

		Commit();
		m_ptr->topology = GL_TRIANGLES;
	}

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

		Commit();
		m_ptr->topology = GL_TRIANGLES;
	}

	Sphere::Sphere(unsigned int xseg, unsigned int yseg)
	{
		for (unsigned int y = 0; y <= yseg; ++y)
		{
			for (unsigned int x = 0; x <= xseg; ++x)
			{
				float u = static_cast<float>(x) / static_cast<float>(xseg);
				float v = static_cast<float>(y) / static_cast<float>(yseg);

				float psi = u * k2Pi;
				float theta = v * kPi;

				float xpos = std::sinf(theta) * std::cosf(psi);
				float ypos = std::cosf(theta);
				float zpos = std::sinf(theta) * std::sinf(psi);

				positions.push_back({ xpos,ypos,zpos });
				texCoords.push_back({ u,v });
				normals.push_back({ xpos,ypos,zpos });
			}
		}

		for (unsigned int y = 0; y < yseg; ++y)
		{
			for (unsigned int x = 0; x < xseg; ++x)
			{
				indices.push_back((y + 1) * (xseg + 1) + x);
				indices.push_back(y       * (xseg + 1) + x);
				indices.push_back(y       * (xseg + 1) + x + 1);

				indices.push_back((y + 1) * (xseg + 1) + x);
				indices.push_back(y       * (xseg + 1) + x + 1);
				indices.push_back((y + 1) * (xseg + 1) + x + 1);
			}
		}

		Commit();
		m_ptr->topology = GL_TRIANGLES;
	}

	Torus::Torus(unsigned int xseg, unsigned int yseg, float r1, float r2)
	{
		for (unsigned int y = 0; y <= yseg; ++y)
		{
			for (unsigned int x = 0; x <= xseg; ++x)
			{
				float u = static_cast<float>(x) / static_cast<float>(xseg);
				float v = static_cast<float>(y) / static_cast<float>(yseg);

				float au = u * k2Pi;
				float av = v * k2Pi;

				float cu = std::cosf(au);
				float su = std::sinf(au);
				float cv = std::sinf(av);
				float sv = std::cosf(av);

				glm::vec3 ru{ cu, 0.0f, su };
				glm::vec3 rv{ cu * cv, sv, su * cv };

				positions.push_back(ru * r1 + rv * r2);
				texCoords.push_back({ u,v });
				normals.push_back(glm::normalize(rv));
			}
		}

		for (unsigned int y = 0; y < yseg; ++y)
		{
			for (unsigned int x = 0; x < xseg; ++x)
			{
				indices.push_back((y + 1) * (xseg + 1) + x);
				indices.push_back(y       * (xseg + 1) + x);
				indices.push_back(y       * (xseg + 1) + x + 1);

				indices.push_back((y + 1) * (xseg + 1) + x);
				indices.push_back(y       * (xseg + 1) + x + 1);
				indices.push_back((y + 1) * (xseg + 1) + x + 1);
			}
		}

		Commit();
		m_ptr->topology = GL_TRIANGLES;
	}
}