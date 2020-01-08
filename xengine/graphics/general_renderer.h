#pragma once
#ifndef XE_GENERAL_RENDERER_H
#define XE_GENERAL_RENDERER_H

#include <camera/camera.h>
#include <mesh/mesh.h>

#include "material.h"
#include "render_command.h"
#include "frame_buffer.h"

namespace xengine
{
	/// collection of basic render methods

	// render a single mesh, based on current shader (uniforms) and ogl settings
	void RenderMesh(Mesh * mesh);

	// render a mesh, given a material (handling shader and all corresponding uniforms, and ogl settings)
	void RenderMesh(Mesh * mesh, Material * material);

	// blit data from one buffer to another (type: GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT, etc.)
	void Blit(FrameBuffer * from, FrameBuffer * to, unsigned int type);
	void Blit(const FrameBuffer & from, FrameBuffer & to, unsigned int type);

	// blit data from one frame buffer to default frame buffer (type: GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT, etc.)
	void Blit(FrameBuffer * from, unsigned int width, unsigned height, unsigned int type);
	void Blit(const FrameBuffer & from, unsigned int width, unsigned height, unsigned int type);
}

#endif // !XE_GENERAL_RENDERER_H
