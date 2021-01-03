#pragma once
#ifndef XE_OBJECT_H
#define XE_OBJECT_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <geometry/aabb.h>

namespace xengine
{
	class GeometryObject
	{
	public:
		GeometryObject();

		// set transform
		virtual void SetPosition(const glm::vec3& position);
		virtual void SetRotation(float radians, const glm::vec3& axis);
		virtual void SetScale(const glm::vec3& scale);

		// make a displacement based on current position
		virtual void Move(const glm::vec3& position);

		// make a rotation based on current posture
		virtual void Rotate(float radians, const glm::vec3& axis);

		// make a scaling based on current size
		virtual void Scale(const glm::vec3& scale);

		// update model's and all its children's transform matrices
		virtual void UpdateTransform();

		// update model's and all its children's transform matrices, given a parent transform matrix
		virtual void UpdateTransform(const glm::mat4& parentTrans);

	public:
		// bounding box
		AABB aabbLocal;
		AABB aabbGlobal;

		// transform
		glm::mat4 transform;
		glm::mat4 prevTrans;
		glm::quat rotation;
		glm::vec3 position;
		glm::vec3 scale;
		bool dirty;
	};
}

#endif // !XE_OBJECT_H
