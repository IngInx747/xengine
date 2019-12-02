#pragma once
#ifndef XE_AABB_H
#define XE_AABB_H

#include <vector>

#include <vendor/glm/common.hpp>

namespace xengine
{
	class AABB
	{
	public:
		AABB();
		AABB(const glm::vec3& vmin, const glm::vec3& vmax);
		AABB(const std::vector<glm::vec3>& vertices);

		// build box from vertices
		void BuildFromVertices(const std::vector<glm::vec3>& vertices);

		// build box based on other box after transformation
		void BuildFromTransform(const AABB& aabb, const glm::mat4& transform);

		// update box
		void UpdateMin(const glm::vec3& umin);
		void UpdateMax(const glm::vec3& umax);

		// union this box with other box
		void UnionAABB(const AABB& aabb);

	public:
		glm::vec3 vmin;
		glm::vec3 vmax;
	};
}

#endif // !XE_AABB_H
