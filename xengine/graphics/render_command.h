#pragma once
#ifndef XE_RENDER_COMMAND_H
#define XE_RENDER_COMMAND_H

#include <memory>

#include <vendor/glm/glm.hpp>

#include <geometry/aabb.h>
#include <mesh/mesh.h>

#include "material.h"

namespace xengine
{
	struct RenderCommand
	{
		AABB aabb;

		glm::mat4 transform;
		glm::mat4 prevTrans;

		Mesh* mesh;
		Material* material;

		RenderCommand();
		RenderCommand(Mesh* mesh, Material* material);
	};
}

#endif // !XE_RENDER_COMMAND_H
