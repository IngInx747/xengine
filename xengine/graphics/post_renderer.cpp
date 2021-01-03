#include "post_renderer.h"

#include <glad/glad.h>

#include <mesh/mesh_manager.h>

#include "shader_manager.h"
#include "render_config.h"
#include "general_renderer.h"

namespace xengine
{
	PostRenderer::PostRenderer()
	{
		m_shader = ShaderManager::LoadGlobalVF("post process", "shaders/effect/effect.quad.vs", "shaders/effect/effect.post_processing.fs");
		m_shader.Bind();
		m_shader.SetUniform("TexSrc", 0);
		m_shader.SetUniform("gMotion", 1);

		m_quad = MeshManager::LoadGlobalPrimitive("quad");
	}

	void PostRenderer::GenerateEffect(const Texture & source)
	{
		source.Bind(0); // TexSrc

		m_shader.Bind();
		m_shader.SetUniform("Sepia", RenderConfig::UseSepia());
		m_shader.SetUniform("Vignette", RenderConfig::UseVignette());

		RenderMesh(&m_quad);

		m_shader.Unbind();
	}
}