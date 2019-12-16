#include "object.h"

#include <vendor/glm/gtc/matrix_transform.hpp>

namespace xengine
{
	GeometryObject::GeometryObject()
		:
		position(0.0f),
		scale(1.0f),
		rotation(glm::quat{}),
		transform(),
		prevTrans(),
		dirty(false)
	{
	}

	void GeometryObject::SetPosition(const glm::vec3& position_)
	{
		position = position_;
		dirty = true;
	}

	void GeometryObject::SetRotation(float angle, const glm::vec3& axis)
	{
		rotation = glm::rotate(glm::quat{}, angle, axis);
		dirty = true;
	}

	void GeometryObject::SetScale(const glm::vec3& scale_)
	{
		scale = scale_;
		dirty = true;
	}

	void GeometryObject::Move(const glm::vec3& displace)
	{
		position += displace;
		dirty = true;
	}

	void GeometryObject::Rotate(float angle, const glm::vec3& axis)
	{
		rotation = glm::rotate(rotation, angle, axis);
		dirty = true;
	}

	void GeometryObject::Scale(const glm::vec3& scale_)
	{
		scale *= scale_;
		dirty = true;
	}

	void GeometryObject::UpdateTransform()
	{
		UpdateTransform(glm::mat4{});
	}

	void GeometryObject::UpdateTransform(const glm::mat4 & parentTrans)
	{
		prevTrans = transform;
		if (!dirty) return;

		// first scale, then rotate, then translation
		glm::mat4 matTranslate = glm::translate(glm::mat4{}, position);
		glm::mat4 matScaling = glm::scale(glm::mat4{}, scale);
		glm::mat4 matRotation = glm::mat4_cast(rotation);
		transform = matTranslate * matRotation * matScaling;

		// transmit transform from parent
		transform = parentTrans * transform;

		// update bounding box
		aabbGlobal.BuildFromTransform(aabbLocal, transform); // update global box

		dirty = false;
	}
}