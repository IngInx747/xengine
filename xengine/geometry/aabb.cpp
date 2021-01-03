#include "aabb.h"

#include <glm/gtx/matrix_operation.hpp>

#include "constant.h"

namespace xengine
{
	AABB::AABB()
		:
		vmin(kMax),
		vmax(kMin)
	{}

	AABB::AABB(const glm::vec3& vmin, const glm::vec3& vmax)
		:
		vmin(vmin),
		vmax(vmax)
	{}

	AABB::AABB(const std::vector<glm::vec3>& vertices)
	{
		if (vertices.size() <= 0)
		{
			vmin = glm::vec3(kMax);
			vmax = glm::vec3(kMin);
		}
		else
		{
			BuildFromVertices(vertices);
		}
	}

	void AABB::BuildFromVertices(const std::vector<glm::vec3>& vertices)
	{
		if (vertices.size() <= 0) return;

		vmin = glm::vec3(kMax);
		vmax = glm::vec3(kMin);

		for (const glm::vec3& vertex : vertices)
		{
			vmin = glm::min(vmin, vertex);
			vmax = glm::max(vmax, vertex);
		}
	}

	void AABB::UpdateMin(const glm::vec3& umin)
	{
		vmin = glm::min(vmin, umin);
	}

	void AABB::UpdateMax(const glm::vec3& umax)
	{
		vmax = glm::max(vmax, umax);
	}

	void AABB::UnionAABB(const AABB& aabb)
	{
		vmin = glm::min(vmin, aabb.vmin);
		vmax = glm::max(vmax, aabb.vmax);
	}

	void AABB::BuildFromTransform(const AABB& aabb, const glm::mat4& transform)
	{
		glm::vec3 umin(kMax), umax(kMin);

		float x0 = aabb.vmin.x, x1 = aabb.vmax.x;
		float y0 = aabb.vmin.y, y1 = aabb.vmax.y;
		float z0 = aabb.vmin.z, z1 = aabb.vmax.z;

		glm::vec3 v000 = { x0, y0, z0 };
		glm::vec3 v001 = { x0, y0, z1 };
		glm::vec3 v010 = { x0, y1, z0 };
		glm::vec3 v011 = { x0, y1, z1 };
		glm::vec3 v100 = { x1, y0, z0 };
		glm::vec3 v101 = { x1, y0, z1 };
		glm::vec3 v110 = { x1, y1, z0 };
		glm::vec3 v111 = { x1, y1, z1 };

		v000 = glm::vec3(transform * glm::vec4(v000, 1.0f));
		v001 = glm::vec3(transform * glm::vec4(v001, 1.0f));
		v010 = glm::vec3(transform * glm::vec4(v010, 1.0f));
		v011 = glm::vec3(transform * glm::vec4(v011, 1.0f));
		v100 = glm::vec3(transform * glm::vec4(v100, 1.0f));
		v101 = glm::vec3(transform * glm::vec4(v101, 1.0f));
		v110 = glm::vec3(transform * glm::vec4(v110, 1.0f));
		v111 = glm::vec3(transform * glm::vec4(v111, 1.0f));

		umin = glm::min(umin, v000);
		umin = glm::min(umin, v001);
		umin = glm::min(umin, v010);
		umin = glm::min(umin, v011);
		umin = glm::min(umin, v100);
		umin = glm::min(umin, v101);
		umin = glm::min(umin, v110);
		umin = glm::min(umin, v111);

		umax = glm::max(umax, v000);
		umax = glm::max(umax, v001);
		umax = glm::max(umax, v010);
		umax = glm::max(umax, v011);
		umax = glm::max(umax, v100);
		umax = glm::max(umax, v101);
		umax = glm::max(umax, v110);
		umax = glm::max(umax, v111);

		vmin = umin;
		vmax = umax;
	}
}
