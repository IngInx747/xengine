#include "eular_camera.h"

#include <vendor/glm/glm.hpp>
#include <vendor/glm/gtx/compatibility.hpp>

#include <geometry/constant.h>

namespace xengine
{
	////////////////////////////////////////////////////////////////
	// Eular camera
	////////////////////////////////////////////////////////////////

	EularCamera::EularCamera()
		:
		Camera(),
		m_targetPosition(0),
		m_worldUp(0, 1, 0)
	{
		yaw = m_targetYaw = -kPi2;
		pitch = m_targetPitch = 0.0f;
	}

	EularCamera::EularCamera(const glm::vec3& position)
		:
		Camera(position, { 0, 0, -1 }, { 0, 1, 0 }),
		m_targetPosition(position),
		m_worldUp(0, 1, 0)
	{
		yaw = m_targetYaw = -kPi2;
		pitch = m_targetPitch = 0.0f;
	}

	EularCamera::EularCamera(const glm::vec3& position, const glm::vec3& forward, const glm::vec3& up)
		:
		Camera(position, forward, up),
		m_targetPosition(position),
		m_worldUp(0, 1, 0)
	{
		// TODO: update pitch and yaw based on forward
		yaw = m_targetYaw = -kPi2;
		pitch = m_targetPitch = 0.0f;
	}

	void EularCamera::Update(float dt)
	{
		// slowly interpolate to target position each frame given some damping factor
		// this gives smooth camera movement that fades out the closer we are to our target
		float damp = 5.0f;
		vPosition = glm::lerp(vPosition, m_targetPosition, glm::clamp(dt * damp, 0.0f, 1.0f));
		yaw = glm::lerp(yaw, m_targetYaw, glm::clamp(dt * damp * 2.0f, 0.0f, 1.0f));
		pitch = glm::lerp(pitch, m_targetPitch, glm::clamp(dt * damp * 2.0f, 0.0f, 1.0f));

		// get new coordinates frame
		float cosp = std::cosf(pitch);
		float sinp = std::sinf(pitch);
		float cosy = std::cosf(yaw);
		float siny = std::sinf(yaw);

		vForward = glm::normalize(glm::vec3{ cosp * cosy, sinp, cosp * siny });
		vRight = glm::normalize(glm::cross(vForward, m_worldUp));
		vUp = glm::cross(vRight, vForward);

		Camera::UpdateView();
		Camera::UpdateProj();
		Camera::UpdateFrustum();
	}

	void EularCamera::OpMove(float dt, unsigned int op)
	{
		float nDsp = movingSpeed * dt; // length of displacement

		switch (op)
		{
		case OP_TYPE::FORWARD:
			m_targetPosition += vForward * nDsp;
			break;
		case OP_TYPE::BACK:
			m_targetPosition -= vForward * nDsp;
			break;
		case OP_TYPE::RIGHT:
			m_targetPosition += vRight * nDsp;
			break;
		case OP_TYPE::LEFT:
			m_targetPosition -= vRight * nDsp;
			break;
		case OP_TYPE::UP:
			m_targetPosition += m_worldUp * nDsp;
			break;
		case OP_TYPE::DOWN:
			m_targetPosition -= m_worldUp * nDsp;
			break;
		}
	}

	static const float kPitchMin = glm::radians(-89.0f);
	static const float kPitchMax = glm::radians(89.0f);

	void EularCamera::OpView(float dx, float dy)
	{
		m_targetYaw += dx * mouseSensitivity;
		m_targetPitch += dy * mouseSensitivity;
		m_targetPitch = glm::clamp(m_targetPitch, kPitchMin, kPitchMax);
	}

	static const float kYawMin = glm::radians(10.0f);
	static const float kYawMax = glm::radians(150.0f);

	void EularCamera::OpZoom(float dx, float dy)
	{
		pFov = glm::clamp(pFov - dy * 0.05f, kYawMin, kYawMax);
	}

	void EularCamera::OpConfig(float dx, float dy, unsigned int op)
	{
		// TODO
	}
}