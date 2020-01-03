#include "render_command_manager.h"

#include <algorithm>

namespace xengine
{
	void RenderCommandManager::Clear()
	{
		m_deferredCommands.clear();
		m_alphaCommands.clear();
		m_processCommands.clear();
		m_forwardCommands.clear();
	}

	void RenderCommandManager::SortOnShaderIndex()
	{
		// deferred render
		std::sort(
			m_deferredCommands.begin(),
			m_deferredCommands.end(),
			[](const RenderCommand& a, const RenderCommand& b) {
			return a.material->shader->ID() < b.material->shader->ID();
		});

		// custom shader
		std::sort(
			m_forwardCommands.begin(),
			m_forwardCommands.end(),
			[](const RenderCommand& a, const RenderCommand& b) {
			return a.material->shader->ID() < b.material->shader->ID();
		});
	}

	void RenderCommandManager::Push(const RenderCommand& command)
	{
		if (command.material->attribute.bBlend)
		{
			command.material->type = Material::FORWARD;
			m_alphaCommands.push_back(command);
		}
		else if (command.material->type == Material::DEFERRED)
		{
			m_deferredCommands.push_back(command);
		}
		else if (command.material->type == Material::FORWARD)
		{
			m_forwardCommands.push_back(command);
		}
		else if (command.material->type == Material::POST_PROCESS)
		{
			m_processCommands.push_back(command);
		}
	}

	std::vector<RenderCommand> RenderCommandManager::ProcessCommands()
	{
		return m_processCommands;
	}

	std::vector<RenderCommand> RenderCommandManager::ForwardCommands(Camera* camera)
	{
		if (!camera) return m_forwardCommands;

		std::vector<RenderCommand> commands;

		for (const RenderCommand& cmd : m_forwardCommands)
		{
			//AABB aabb; aabb.BuildFromTransform(cmd.mesh->aabb, cmd.transform);

			if (camera->IntersectFrustum(cmd.aabb))
				commands.push_back(cmd);
		}

		return commands;
	}

	std::vector<RenderCommand> RenderCommandManager::DeferredCommands(Camera* camera)
	{
		if (!camera) return m_deferredCommands;

		std::vector<RenderCommand> commands;

		for (const RenderCommand& cmd : m_deferredCommands)
		{
			//AABB aabb; aabb.BuildFromTransform(cmd.mesh->aabb, cmd.transform);

			if (camera->IntersectFrustum(cmd.aabb))
				commands.push_back(cmd);
		}

		return commands;
	}

	std::vector<RenderCommand> RenderCommandManager::AlphaCommands(Camera* camera)
	{
		if (!camera) return m_alphaCommands;

		std::vector<RenderCommand> commands;

		for (const RenderCommand& cmd : m_alphaCommands)
		{
			//AABB aabb; aabb.BuildFromTransform(cmd.mesh->aabb, cmd.transform);

			if (camera->IntersectFrustum(cmd.aabb))
				commands.push_back(cmd);
		}

		return commands;
	}

	std::vector<RenderCommand> RenderCommandManager::ShadowCastCommands()
	{
		std::vector<RenderCommand> commands;

		for (const RenderCommand& cmd : m_deferredCommands)
		{
			if (cmd.material->attribute.bShadowCast)
				commands.push_back(cmd);
		}

		for (const RenderCommand& cmd : m_forwardCommands)
		{
			if (cmd.material->attribute.bShadowCast)
				commands.push_back(cmd);
		}

		return commands;
	}
}