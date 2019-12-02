#include "downsample_renderer.h"

#include <vendor/glad/glad.h>

#include <mesh/mesh_manager.h>

#include "../shader/shader_manager.h"
#include "general_renderer.h"

namespace xengine
{
	////////////////////////////////////////////////////////////////
	// Util
	////////////////////////////////////////////////////////////////

	struct FuncDownsample
	{
		FuncDownsample(Shader* shader, Mesh* canvas)
			:
			shader(shader),
			canvas(canvas)
		{}

		void operator () (Texture* src, FrameBuffer* dst);

		Shader* shader;
		Mesh* canvas;
	};

	void FuncDownsample::operator() (Texture* src, FrameBuffer* dst)
	{
		dst->Bind();
		glViewport(0, 0, dst->Width(), dst->Height());
		glClear(GL_COLOR_BUFFER_BIT);

		src->Bind(0);
		shader->Use();
		GeneralRenderer::RenderMesh(canvas);
	}

	////////////////////////////////////////////////////////////////
	// Pass
	////////////////////////////////////////////////////////////////

	DownsampleRenderer::DownsampleRenderer()
	{
		m_target0.GenerateColorAttachments(1, 1, GL_HALF_FLOAT, 1);
		m_target1.GenerateColorAttachments(1, 1, GL_HALF_FLOAT, 1);
		m_target2.GenerateColorAttachments(1, 1, GL_HALF_FLOAT, 1);
		m_target3.GenerateColorAttachments(1, 1, GL_HALF_FLOAT, 1);

		m_output0 = m_target0.GetColorAttachment(0);
		m_output1 = m_target1.GetColorAttachment(0);
		m_output2 = m_target2.GetColorAttachment(0);
		m_output3 = m_target3.GetColorAttachment(0);

		m_shader = ShaderManager::LoadVertFragShader("down sample", "shaders/screen_quad.vs", "shaders/post/down_sample.fs");
		m_shader->Use();
		m_shader->SetUniform("TexSrc", 0);

		m_quad = MeshManager::LoadPrimitive("quad");
	}

	void DownsampleRenderer::Resize(unsigned int width, unsigned int height)
	{
		unsigned int w2 = static_cast<unsigned int>(width * 0.5f);
		unsigned int w4 = static_cast<unsigned int>(width * 0.25f);
		unsigned int w8 = static_cast<unsigned int>(width * 0.125f);
		unsigned int w16 = static_cast<unsigned int>(width * 0.0675f);
		unsigned int h2 = static_cast<unsigned int>(height * 0.5f);
		unsigned int h4 = static_cast<unsigned int>(height * 0.25f);
		unsigned int h8 = static_cast<unsigned int>(height * 0.125f);
		unsigned int h16 = static_cast<unsigned int>(height * 0.0675f);

		m_target0.Resize(w2, h2);
		m_target1.Resize(w4, h4);
		m_target2.Resize(w8, h8);
		m_target3.Resize(w16, h16);
	}

	void DownsampleRenderer::Generate(Texture* source)
	{
		FuncDownsample func(m_shader, m_quad);
		func(source,    &m_target0); // src -> 0
		func(m_output0, &m_target1); // 0 -> 1
		func(m_output1, &m_target2); // 1 -> 2
		func(m_output2, &m_target3); // 2 -> 3
	}
}