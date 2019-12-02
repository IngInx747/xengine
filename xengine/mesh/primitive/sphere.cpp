#include "sphere.h"

#include <vendor/glad/glad.h>
#include <vendor/glm/glm.hpp>

#include <geometry/constant.h>

namespace xengine
{
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

		aabb.BuildFromVertices(positions);

		topology = GL_TRIANGLES;

		CommitData();
	}
}
