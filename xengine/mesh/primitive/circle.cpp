#include "circle.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <geometry/constant.h>

namespace xengine
{
	Circle::Circle(unsigned int pseg, unsigned int rseg)
	{
		for (unsigned int y = 0; y <= rseg; ++y)
		{
			for (unsigned int x = 0; x <= pseg; ++x)
			{
				float u = static_cast<float>(x) / pseg;
				float v = static_cast<float>(y) / rseg;
				float psi = u * k2Pi;
				float xpos = std::cosf(psi);
				float ypos = std::sinf(psi);

				positions.push_back({ xpos * v, ypos * v, 0.0f });
				texCoords.push_back({ u,v });
				normals.push_back({ 0.0f, 1.0f, 0.0f });
			}
		}

		// indices are exactly the same as for the plane, only the positions differ for a circle
		bool oddRow = false;

		for (unsigned int y = 0; y < rseg; ++y)
		{
			if (!oddRow) // even rows: y = 0, 2, 4, ...
			{
				for (unsigned int x = 0; x <= pseg; ++x)
				{
					indices.push_back(y       * (pseg + 1) + x);
					indices.push_back((y + 1) * (pseg + 1) + x);
				}
			}
			else
			{
				for (unsigned int x = pseg; x >= 0; --x)
				{
					indices.push_back((y + 1) * (pseg + 1) + x);
					indices.push_back(y       * (pseg + 1) + x);
				}
			}
			oddRow = !oddRow;
		}

		aabb.BuildFromVertices(positions);

		topology = GL_TRIANGLE_STRIP;

		CommitData();
	}
}
