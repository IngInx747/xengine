#pragma once
#ifndef XE_SKYBOX_H
#define XE_SKYBOX_H

#include <memory>

#include <graphics/shader.h>
#include <graphics/texture.h>

#include "model.h"

namespace xengine
{
	class Skybox : public Model
	{
	public:
		Skybox();

		// set background cube map
		void SetCubeMap(const CubeMap& cubemap);
	};
}

#endif // !XE_SKYBOX_H
