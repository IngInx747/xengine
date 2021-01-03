#pragma once
#ifndef XE_LIGHT_H
#define XE_LIGHT_H

#include <glm/common.hpp>

#include "shadow.h"

namespace xengine
{
	class Light
	{
	public:
		Light();
		Light(const glm::vec3& color);

	public:
		glm::vec3 color;
	};

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

	class PointLight : public Light
	{
	public:
		PointLight();

	public:
		// basic
		glm::vec3 position;
		float intensity;

		// light volumn
		float radius;
		bool useVolume;
	};
}

#endif // !XE_LIGHT_H
