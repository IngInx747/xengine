#pragma once
#ifndef XE_POINT_LIGHT_H
#define XE_POINT_LIGHT_H

#include <vendor/glm/glm.hpp>

#include "light.h"

namespace xengine
{
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

#endif // !XE_POINT_LIGHT_H
