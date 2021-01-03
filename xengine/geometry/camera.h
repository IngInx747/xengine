#pragma once
#ifndef XE_CAMERA_H
#define XE_CAMERA_H

#include <glm/glm.hpp>

#include <geometry/frustum.h>
#include <geometry/aabb.h>

namespace xengine
{
	class Camera
	{
	public:
		Camera();
		Camera(const glm::vec3& position);
		Camera(const glm::vec3& position, const glm::vec3& forward, const glm::vec3& up);

		virtual void Update(float dt);

		void SetCamera(const glm::vec3& position);
		void SetCamera(const glm::vec3& position, const glm::vec3& forward, const glm::vec3& up);

		void SetView(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up);

		void SetProjPerspective(float fov, float aspect, float zNear, float zFar);
		void SetProjOrtho(float left, float right, float bottom, float top, float zNear, float zFar);

		void UpdateView();
		void UpdateProj();
		void UpdateFrustum();

		bool IntersectFrustum(const glm::vec3& point, float radius) const;
		bool IntersectFrustum(const AABB& aabb) const;

		float FrustumHeightAtDistance(float distance) const;
		float DistanceAtFrustumHeight(float frustumHeight) const;

		inline const glm::vec3& GetPosition() const { return vPosition; }
		inline const glm::vec3& GetForward() const { return vForward; }
		inline const glm::vec3& GetUp() const { return vUp; }
		inline const glm::vec3& GetRight() const { return vRight; }
		inline const glm::mat4& GetView() const { return matView; }
		inline const glm::mat4& GetPrevView() const { return matPrevView; }
		inline const glm::mat4& GetProjection() const { return matProjection; }

	protected:
		void updateProjPerspective();
		void updateProjOrtho();

		void updateFrustumPerspective();
		void updateFrustumOrtho();

	protected:
		// frustum for on-render trimming
		Frustum frustum;

		// view and projection
		glm::mat4 matView;
		glm::mat4 matPrevView;
		glm::mat4 matProjection;

		// camera position
		glm::vec3 vPosition;

		// camera gesture
		glm::vec3 vForward;
		glm::vec3 vUp;
		glm::vec3 vRight;

		// general params
		float zNear;
		float zFar;

		// orthographic params
		float oLeft;
		float oRight;
		float oBottom;
		float oTop;

		// perspective params
		float pFov; // field of view (radian)
		float pAspect; // width / height

		// if the camera in perspective mode or orthogonal
		bool isProjPers;
	};

	class EularCamera : public Camera
	{
	public:
		enum OP_TYPE
		{
			FORWARD,
			BACK,
			LEFT,
			RIGHT,
			UP,
			DOWN,
		};

	public:
		EularCamera();
		EularCamera(const glm::vec3& position);
		EularCamera(const glm::vec3& position, const glm::vec3& forward, const glm::vec3& up);

		// update camera gesture
		void Update(float dt);

		// modify camera parameters
		void OpMove(float dt, unsigned int op);
		void OpView(float dx, float dy);
		void OpZoom(float dx, float dy);
		void OpConfig(float dx, float dy, unsigned int op);

	public:
		float yaw; // (radian)
		float pitch; // (radian)

		float movingSpeed = 10.0f;
		float mouseSensitivity = 0.00175f;

	private:
		float m_targetYaw; // (radian)
		float m_targetPitch; // (radian)

		glm::vec3 m_targetPosition;
		glm::vec3 m_worldUp;
	};
}

#endif // !XE_CAMERA_H
