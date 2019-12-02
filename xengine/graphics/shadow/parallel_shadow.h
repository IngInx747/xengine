#pragma once
#ifndef XE_PARALLEL_SHADOW_H
#define XE_PARALLEL_SHADOW_H

#include <camera/camera.h>

#include "shadow.h"

namespace xengine
{
	class ParallelShadow : public Shadow
	{
	public:
		ParallelShadow();

		// generate a shadow map for parallel light
		void GenerateShadowMap(unsigned int width, unsigned height);

		// update light view based on light direction and camera position
		void UpdateView(const glm::vec3& lightDir, const glm::vec3& camPos);

		// set out-of-view depth value (1: 1.0, 0: 0.0)
		void SetBorderDepth(bool flag);

		inline const glm::mat4 GetView() const { return camera.GetView(); }
		inline const glm::mat4 GetProj() const { return camera.GetProjection(); }
		inline const glm::mat4 GetViewProj() const { return viewProj; }
		inline Camera* GetCamera() { return &camera; }

	protected:
		Camera camera;
		glm::mat4 viewProj; // pre-calculated projection * view
	};
}

#endif // !XE_PARALLEL_SHADOW_H
