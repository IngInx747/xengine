#include "camera.h"

#include <vendor/glm/glm.hpp>
#include <vendor/glm/gtc/matrix_transform.hpp>

#include <geometry/constant.h>

namespace xengine
{
	////////////////////////////////////////////////////////////////
	// Camera
	////////////////////////////////////////////////////////////////

	Camera::Camera()
		:
		vPosition(0.0f, 0.0f, 0.0f),
		vForward(0.0f, 0.0f, -1.0f),
		vUp(0.0f, 1.0f, 0.0f),
		vRight(1.0f, 0.0f, 0.0f)
	{
		SetProjPerspective(kPi4, 1.0f, 0.1f, 100.0f);
		Update(0);
	}

	Camera::Camera(const glm::vec3 & position)
		:
		vPosition(position),
		vForward(0.0f, 0.0f, -1.0f),
		vUp(0.0f, 1.0f, 0.0f),
		vRight(1.0f, 0.0f, 0.0f)
	{
		SetProjPerspective(kPi4, 1.0f, 0.1f, 100.0f);
	}

	Camera::Camera(const glm::vec3& position, const glm::vec3& forward, const glm::vec3& up)
		:
		vPosition(position),
		vForward(glm::normalize(forward)),
		vUp(glm::normalize(up)),
		vRight(1.0f, 0.0f, 0.0f)
	{
		SetProjPerspective(kPi4, 1.0f, 0.1f, 100.0f);
		Update(0);
	}

	void Camera::SetCamera(const glm::vec3 & position)
	{
		vPosition = position;
		Update(0);
	}

	void Camera::SetCamera(const glm::vec3 & position, const glm::vec3 & forward, const glm::vec3 & up)
	{
		vPosition = position;
		vForward = glm::normalize(forward);
		vUp = glm::normalize(up);
		Update(0);
	}

	void Camera::SetView(const glm::vec3 & eye, const glm::vec3 & center, const glm::vec3 & up)
	{
		vPosition = eye;
		vForward = glm::normalize(center - eye);
		vUp = glm::normalize(up);
		Update(0);
	}

	void Camera::SetProjPerspective(float fov, float aspect, float near, float far)
	{
		isProjPers = true;
		matProjection = glm::perspective(fov, aspect, near, far);
		zNear = near;
		zFar = far;
		pFov = fov;
		pAspect = aspect;
	}

	void Camera::SetProjOrtho(float left, float right, float bottom, float top, float near, float far)
	{
		isProjPers = false;
		matProjection = glm::ortho(left, right, bottom, top, near, far);
		zNear = near;
		zFar = far;
		oLeft = left;
		oRight = right;
		oBottom = bottom;
		oTop = top;
	}

	void Camera::Update(float dt)
	{
		vRight = glm::normalize(glm::cross(vForward, vUp));
		vUp = glm::normalize(glm::cross(vRight, vForward));

		UpdateView();
		UpdateFrustum();
	}

	void Camera::UpdateView()
	{
		matPrevView = matView;
		matView = glm::lookAt(vPosition, vPosition + vForward, vUp);
	}

	void Camera::UpdateProj()
	{
		if (isProjPers) updateProjPerspective();
		else updateProjOrtho();
	}

	void Camera::UpdateFrustum()
	{
		if (isProjPers) updateFrustumPerspective();
		else updateFrustumOrtho();
	}

	void Camera::updateProjPerspective()
	{
		matProjection = glm::perspective(pFov, pAspect, zNear, zFar);
	}

	void Camera::updateProjOrtho()
	{
		matProjection = glm::ortho(oLeft, oRight, oBottom, oTop, zNear, zFar);
	}

	void Camera::updateFrustumPerspective()
	{
		float tanFov = 2.0f * std::tanf(pFov * 0.5f);
		float nearHeight = tanFov * zNear;
		float nearWidth = nearHeight * pAspect;
		float farHeight = tanFov * zFar;
		float farWidth = farHeight * pAspect;

		glm::vec3 nearCenter = vPosition + vForward * zNear;
		glm::vec3 farCenter = vPosition + vForward * zFar;

		glm::vec3 v;
		// left plane
		v = (nearCenter - vRight * nearWidth * 0.5f) - vPosition;
		frustum.left.Set(glm::cross(vUp, glm::normalize(v)), nearCenter - vRight * nearWidth * 0.5f);
		// right plane
		v = (nearCenter + vRight * nearWidth  * 0.5f) - vPosition;
		frustum.right.Set(glm::cross(glm::normalize(v), vUp), nearCenter + vRight * nearWidth * 0.5f);
		// top plane
		v = (nearCenter + vUp * nearHeight * 0.5f) - vPosition;
		frustum.top.Set(glm::cross(vRight, glm::normalize(v)), nearCenter + vUp * nearHeight * 0.5f);
		// bottom plane
		v = (nearCenter - vUp * nearHeight * 0.5f) - vPosition;
		frustum.bottom.Set(glm::cross(glm::normalize(v), vRight), nearCenter - vUp * nearHeight * 0.5f);
		// near plane
		frustum.near.Set(-vForward, nearCenter);
		// far plane
		frustum.far.Set(vForward, farCenter);
	}

	void Camera::updateFrustumOrtho()
	{
		// left plane
		frustum.left.Set(-vRight, vPosition + vRight * oLeft);
		// right plane
		frustum.right.Set(vRight, vPosition + vRight * oRight);
		// top plane
		frustum.top.Set(vUp, vPosition + vUp * oTop);
		// bottom plane
		frustum.bottom.Set(-vUp, vPosition + vUp * oBottom);
		// near plane
		frustum.near.Set(-vForward, vPosition + vForward * zNear);
		// far plane
		frustum.far.Set(vForward, vPosition + vForward * zFar);
	}

	bool Camera::IntersectFrustum(const glm::vec3 & point, float radius) const
	{
		return frustum.Intersect(point, radius);
	}

	bool Camera::IntersectFrustum(const AABB & aabb) const
	{
		return frustum.Intersect(aabb.vmin, aabb.vmax);
	}

	float Camera::FrustumHeightAtDistance(float distance) const
	{
		if (isProjPers)
			return 2.0f * distance * std::tanf(glm::radians(pFov * 0.5f));
		else
			return frustum.top.D;
	}

	float Camera::DistanceAtFrustumHeight(float frustumHeight) const
	{
		if (isProjPers)
			return frustumHeight * 0.5f / std::tanf(glm::radians(pFov * 0.5f));
		else
			return frustum.near.D;
	}
}