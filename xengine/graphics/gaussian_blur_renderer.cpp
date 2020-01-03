#include "gaussian_blur_renderer.h"

#include <vendor/glad/glad.h>

#include <mesh/mesh_manager.h>

#include "shader_manager.h"
#include "general_renderer.h"

namespace xengine
{
	////////////////////////////////////////////////////////////////
	// Util
	////////////////////////////////////////////////////////////////

	void gaussian_blur_pingpong_op(Texture* src, FrameBuffer* dst, FrameBuffer* medium, Shader* shader, unsigned int count)
	{
		// pick pre-defined render targets for blur based on render size
		FrameBuffer* rt_horizontal = medium;

		// use destination as vertical render target of ping-pong algorithm
		FrameBuffer* rt_vertical = dst;

		glViewport(0, 0, dst->Width(), dst->Height());

		bool flag = true; // last is horizontal
		shader->Bind();

		for (unsigned int i = 0; i < count; ++i, flag = !flag)
		{
			shader->SetUniform("bHorizontal", flag);

			if (i == 0)
				src->Bind(0);
			else if (flag)
				rt_vertical->GetColorAttachment(0)->Bind(0);
			else
				rt_horizontal->GetColorAttachment(0)->Bind(0);

			if (flag) rt_horizontal->Bind();
			else rt_vertical->Bind();

			glDrawArrays(GL_POINTS, 0, 1);
		}
	}

	////////////////////////////////////////////////////////////////
	// Pass
	////////////////////////////////////////////////////////////////

	GaussianBlurRenderer::GaussianBlurRenderer()
	{
		m_target0.GenerateColorAttachments(1, 1, GL_HALF_FLOAT, 1);
		m_target1.GenerateColorAttachments(1, 1, GL_HALF_FLOAT, 1);

		m_output0 = m_target0.GetColorAttachment(0);
		m_output1 = m_target1.GetColorAttachment(0);

		m_medium0.GenerateColorAttachments(1, 1, GL_HALF_FLOAT, 1);
		m_medium1.GenerateColorAttachments(1, 1, GL_HALF_FLOAT, 1);

		m_genShader.AttachVertexShader(ReadShaderSource("shaders/effect/quad.vs"));
		m_genShader.AttachGeometryShader(ReadShaderSource("shaders/effect/quad.gs"));
		m_genShader.AttachFragmentShader(ReadShaderSource("shaders/effect/effect.gen.gaussian_blur.fs"));
		m_genShader.GenerateAndLink();

		m_genShader.Bind();
		m_genShader.SetUniform("TexSrc", 0);
	}

	void GaussianBlurRenderer::Resize(unsigned int width, unsigned int height)
	{
		unsigned int w8 = static_cast<unsigned int>(width * 0.125f);
		unsigned int w16 = static_cast<unsigned int>(width * 0.0675f);
		unsigned int h8 = static_cast<unsigned int>(height * 0.125f);
		unsigned int h16 = static_cast<unsigned int>(height * 0.0675f);

		m_target0.Resize(w8, h8);
		m_target1.Resize(w16, h16);

		m_medium0.Resize(w8, h8);
		m_medium1.Resize(w16, h16);
	}

	void GaussianBlurRenderer::Generate(Texture * source)
	{
		gaussian_blur_pingpong_op(source, &m_target0, &m_medium0, &m_genShader, 4);
		gaussian_blur_pingpong_op(source, &m_target1, &m_medium1, &m_genShader, 4);
	}
}