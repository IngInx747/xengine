#pragma once
#ifndef XE_SKYBOX_H
#define XE_SKYBOX_H

#include <memory>

#include <graphics/shader/shader.h>
#include <graphics/texture/texture.h>

#include "model.h"

namespace xengine
{
	class Skybox : public Model
	{
	public:
		Skybox();

		// set background cube map
		void SetCubeMap(CubeMap* cubemap);
	};
}

#endif // !XE_SKYBOX_H
