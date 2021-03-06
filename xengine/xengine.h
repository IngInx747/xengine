#pragma once
#ifndef XENGINE_H
#define XENGINE_H

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>

#include <utility/log.h>
#include <mesh/mesh.h>
#include <mesh/primitive.h>
#include <mesh/mesh_manager.h>
#include <model/model.h>
#include <model/skybox.h>
#include <model/model_manager.h>
#include <scene/scene.h>
#include <geometry/camera.h>
#include <graphics/shader_manager.h>
#include <graphics/texture_manager.h>
#include <graphics/material_manager.h>
#include <graphics/renderer.h>
#include <graphics/ibl_renderer.h>
#include <ui/ui.h>

namespace xengine
{
	// initialize resource, renderer, etc.
	void xe_initialize(GLFWwindow* & window, unsigned int width, unsigned int height, const std::string& title);

	// clear resource
	void xe_terminate();
}

#endif // !XENGINE_H
