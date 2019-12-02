#include "light.h"

namespace xengine
{
	Light::Light()
		:
		color(1.0f)
	{}

	Light::Light(const glm::vec3& color)
		:
		color(color)
	{}
}