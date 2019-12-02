#pragma once
#ifndef XE_LIGHT_H
#define XE_LIGHT_H

#include <vendor/glm/common.hpp>

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
}

#endif // !XE_LIGHT_H
