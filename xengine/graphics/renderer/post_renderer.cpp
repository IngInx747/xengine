#include "post_renderer.h"

#include <vendor/glad/glad.h>

#include <mesh/mesh_manager.h>

#include "../shader/shader_manager.h"
#include "render_config.h"
#include "general_renderer.h"

namespace xengine
{
	PostRenderer::PostRenderer()
	{
		m_target.GenerateColorAttachments(1, 1, GL_UNSIGNED_BYTE, 1);

		m_shader = ShaderManager::LoadVertFragShader("post process", "shaders/screen_quad.vs", "shaders/post_processing.fs");
		m_shader->Use();
		m_shader->SetUniform("TexSrc", 0);
		m_shader->SetUniform("TexBloom1", 1);
		m_shader->SetUniform("TexBloom2", 2);
		m_shader->SetUniform("TexBloom3", 3);
		m_shader->SetUniform("TexBloom4", 4);
		m_shader->SetUniform("gMotion", 5);

		m_quad = MeshManager::LoadPrimitive("quad");
	}

	void PostRenderer::Resize(unsigned int width, unsigned int height)
	{
		m_target.Resize(width, height);
	}

	void PostRenderer::GenerateEffect(Texture* source, Texture* motion, Texture* bloom1, Texture* bloom2, Texture* bloom3, Texture* bloom4)
	{
		m_target.Bind();
		glViewport(0, 0, m_target.Width(), m_target.Height());
		glClear(GL_COLOR_BUFFER_BIT);

		// bind input texture data
		source->Bind(0); // TexSrc
		bloom1->Bind(1);
		bloom2->Bind(2);
		bloom3->Bind(3);
		bloom4->Bind(4);
		motion->Bind(5); // gMotion

		// set settings 
		m_shader->Use();
		m_shader->SetUniform("Sepia", RenderConfig::UseSepia());
		m_shader->SetUniform("Vignette", RenderConfig::UseVignette());
		m_shader->SetUniform("Bloom", RenderConfig::UseBloom());
		m_shader->SetUniform("MotionBlur", RenderConfig::UseMotionBlur());
		m_shader->SetUniform("MotionScale", 1.0f);
		m_shader->SetUniform("MotionSamples", 16);

		GeneralRenderer::RenderMesh(m_quad);
	}
}