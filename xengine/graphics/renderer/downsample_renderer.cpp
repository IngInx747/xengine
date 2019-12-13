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

	void downsample_op(Texture* src, FrameBuffer* dst, Shader* shader)
	{
		dst->Bind();
		glViewport(0, 0, dst->Width(), dst->Height());
		glClear(GL_COLOR_BUFFER_BIT);

		src->Bind(0);
		shader->Bind();
		glDrawArrays(GL_POINTS, 0, 1);
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

		m_genShader.AttachVertexShader(ReadShaderSource("shaders/effect/quad.vs"));
		m_genShader.AttachGeometryShader(ReadShaderSource("shaders/effect/quad.gs"));
		m_genShader.AttachFragmentShader(ReadShaderSource("shaders/effect/effect.gen.downsample.fs"));
		m_genShader.GenerateAndLink();

		m_genShader.Bind();
		m_genShader.SetUniform("TexSrc", 0);
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
		downsample_op(source,    &m_target0, &m_genShader); // src -> 0
		downsample_op(m_output0, &m_target1, &m_genShader); // 0 -> 1
		downsample_op(m_output1, &m_target2, &m_genShader); // 1 -> 2
		downsample_op(m_output2, &m_target3, &m_genShader); // 2 -> 3
	}
}