#pragma once
#ifndef XENGINE_H
#define XENGINE_H

#include <vendor/glad/glad.h>
#include <vendor/GLFW/glfw3.h>
#include <vendor/glm/glm.hpp>

#include <utility/log.h>
#include <mesh/mesh.h>
#include <mesh/primitive.h>
#include <mesh/mesh_manager.h>
#include <model/model.h>
#include <model/skybox.h>
#include <model/model_manager.h>
#include <scene/scene.h>
#include <camera/eular_camera.h>
#include <graphics/shader/shader_manager.h>
#include <graphics/texture/texture_manager.h>
#include <graphics/material/material_manager.h>
#include <graphics/renderer/renderer.h>
#include <ui/ui.h>

namespace xengine
{
	// initialize resource, renderer, etc.
	void xe_initialize(GLFWwindow* & window, unsigned int width, unsigned int height, const std::string& title);

	// clear resource
	void xe_terminate();
}

#endif // !XENGINE_H
