#pragma once
#ifndef XE_EULAR_CAMERA_H
#define XE_EULAR_CAMERA_H

#include "camera.h"

namespace xengine
{
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

#endif // !XE_EULAR_CAMERA_H
