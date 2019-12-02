#include "point_light.h"

namespace xengine
{
	PointLight::PointLight()
		:
		Light(),
		position(0.0f, 0.0f, 0.0f),
		intensity(1.0f),
		radius(1.0f),
		useVolume(false)
	{}
}