#include "torus.h"

#include <vendor/glad/glad.h>
#include <vendor/glm/glm.hpp>

#include <geometry/constant.h>

namespace xengine
{
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
				glm::vec3 rv{cu * cv, sv, su * cv};

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

		aabb.BuildFromVertices(positions);

		topology = GL_TRIANGLES;

		CommitGpuData();
	}
}
