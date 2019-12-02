#pragma once
#ifndef XE_PARALLEL_LIGHT
#define XE_PARALLEL_LIGHT

#include <memory>

#include <vendor/glm/glm.hpp>

#include "../shadow/parallel_shadow.h"

#include "light.h"

namespace xengine
{
	class ParallelLight : public Light
	{
	public:
		ParallelLight();
		ParallelLight(const glm::vec3& direction, float intensity = 1.0f);

		// update shadow view after setting light direction and look-at center
		void UpdateShadowView(const glm::vec3& center);

		// set light only to light up a rectangle area, not whole plane (only available when shadow is ON)
		void SetRegional(bool regional);

	public:
		// basic
		glm::vec3 direction;
		float intensity;

		// shadow (TODO: cascaded shadow map)
		ParallelShadow shadow;
		bool useShadowCast;
	};
}

#endif // !XE_PARALLEL_LIGHT
