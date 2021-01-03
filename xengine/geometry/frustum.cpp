#include "frustum.h"

#include <glm/glm.hpp>

namespace xengine
{
	////////////////////////////////////////////////////////////////
	// Frustum
	//
	// Definition:
	// \pi_i: n_i \dot x + d_i = 0 (i = 0, 1, 2, 3, 4, 5)
	// n_i: unit normal vector of ith plane
	// d_i: negative distance between origin and plane
	// x  : arbitrary point on ith plane
	//
	// Sign of n_i \dot x, where '|' labels ith plane:
	// ------  |-(n)->  ++++++
	//
	// Frustum-point intersection:
	// x is outside of \pi_i <=> n \dot x > 0
	////////////////////////////////////////////////////////////////

	Frustum::Frustum()
	{
	}

	Frustum::Frustum(const Frustum& other)
	{
		planes[0] = other.planes[0];
		planes[1] = other.planes[1];
		planes[2] = other.planes[2];
		planes[3] = other.planes[3];
		planes[4] = other.planes[4];
		planes[5] = other.planes[5];
	}

	Frustum::~Frustum()
	{
	}

	void Frustum::Plane::Set(const glm::vec3& nor, const glm::vec3& point)
	{
		normal = glm::normalize(nor);
		D = -glm::dot(normal, point);
	}

	float Frustum::Plane::Distance(const glm::vec3& point) const
	{
		return glm::dot(normal, point) + D;
	}

	bool Frustum::Intersect(const glm::vec3& point, float radius) const
	{
		for (int i = 0; i < 6; ++i)
		{
			if (planes[i].Distance(point) > radius)
				return false;
		}

		return true;
	}

	bool Frustum::Intersect(const glm::vec3& vmin, const glm::vec3& vmax) const
	{
		for (int i = 0; i < 6; ++i)
		{
			glm::vec3 positive = vmin;

			if (planes[i].normal.x <= 0) positive.x = vmax.x;
			if (planes[i].normal.y <= 0) positive.y = vmax.y;
			if (planes[i].normal.z <= 0) positive.z = vmax.z;

			if (planes[i].Distance(positive) > 0) return false;
		}

		return true;
	}
}