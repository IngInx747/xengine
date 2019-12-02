#pragma once
#ifndef XE_RENDER_COMMAND_MANAGER_H
#define XE_RENDER_COMMAND_MANAGER_H

#include <vector>
#include <memory>
#include <unordered_map>

#include <vendor/glm/glm.hpp>

#include <camera/camera.h>
#include <mesh/mesh.h>

#include "../material/material.h"
#include "../frame_buffer/frame_buffer.h"

#include "render_command.h"

namespace xengine
{
	class RenderCommandManager
	{
	public:
		void Clear();

		void Push(const RenderCommand& command);

		// sort is optional (avoid shader switch)
		void SortOnShaderIndex();

		std::vector<RenderCommand> ForwardCommands(Camera* camera = nullptr);
		std::vector<RenderCommand> DeferredCommands(Camera* camera = nullptr);
		std::vector<RenderCommand> AlphaCommands(Camera* camera = nullptr);
		std::vector<RenderCommand> ProcessCommands();
		std::vector<RenderCommand> ShadowCastCommands();

	private:
		std::vector<RenderCommand> m_forwardCommands;
		std::vector<RenderCommand> m_deferredCommands;
		std::vector<RenderCommand> m_alphaCommands; // transparent object
		std::vector<RenderCommand> m_processCommands; // post effect processing
	};
}

#endif // !XE_RENDER_COMMAND_MANAGER_H
