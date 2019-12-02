#pragma once
#ifndef XE_FRUSTUM_H
#define XE_FRUSTUM_H

#include <memory>

#include <vendor/glm/common.hpp>

namespace xengine
{
	class Frustum
	{
	public:
		// oriental plane
		struct Plane
		{
			glm::vec3 normal;
			float D;

			void Set(const glm::vec3& normal, const glm::vec3& point);
			float Distance(const glm::vec3& point) const;
		};

	public:
		Frustum();
		Frustum(const Frustum& other);
		~Frustum();

		bool Intersect(const glm::vec3& point, float radius) const;
		bool Intersect(const glm::vec3& vmin, const glm::vec3& vmax) const;

	public:
		union
		{
			Plane planes[6];
			struct { Plane left, right, top, bottom, near, far; };
		};
	};
}

#endif // !XE_FRUSTUM_H
