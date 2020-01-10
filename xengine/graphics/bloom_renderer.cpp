#include "bloom_renderer.h"

#include <vendor/glad/glad.h>

#include <mesh/mesh_manager.h>

#include "shader_manager.h"
#include "general_renderer.h"

namespace xengine
{
	////////////////////////////////////////////////////////////////
	// Util
	////////////////////////////////////////////////////////////////

	static void bloom_blur_pingpong_op(
		const Texture & src,
		FrameBuffer & dst,
		FrameBuffer & medium,
		Shader & shader,
		Mesh & quad,
		unsigned int count)
	{
		// pick pre-defined render targets for blur based on render size
		FrameBuffer& rt_horizontal = medium;

		// use destination as vertical render target of ping-pong algorithm
		FrameBuffer& rt_vertical = dst;

		glViewport(0, 0, dst.Width(), dst.Height());

		bool flag = true; // last is horizontal
		shader.Bind();

		for (unsigned int i = 0; i < count; ++i, flag = !flag)
		{
			shader.SetUniform("bHorizontal", flag);

			if (i == 0)
				src.Bind(0);
			else if (flag)
				rt_vertical.GetColorAttachment(0).Bind(0);
			else
				rt_horizontal.GetColorAttachment(0).Bind(0);

			if (flag) rt_horizontal.Bind();
			else rt_vertical.Bind();

			RenderMesh(&quad);
		}
	}

	////////////////////////////////////////////////////////////////
	// Pass
	////////////////////////////////////////////////////////////////

	BloomRenderer::BloomRenderer()
	{
		m_target0.GenerateColorAttachments(1, 1, GL_HALF_FLOAT, 1); // 1/2 target
		m_target1.GenerateColorAttachments(1, 1, GL_HALF_FLOAT, 1); // 1/4 target
		m_target2.GenerateColorAttachments(1, 1, GL_HALF_FLOAT, 1); // 1/8 target
		m_target3.GenerateColorAttachments(1, 1, GL_HALF_FLOAT, 1); // 1/16 target

		m_output0 = m_target0.GetColorAttachment(0);
		m_output1 = m_target1.GetColorAttachment(0);
		m_output2 = m_target2.GetColorAttachment(0);
		m_output3 = m_target3.GetColorAttachment(0);

		m_medium0.GenerateColorAttachments(1, 1, GL_HALF_FLOAT, 1); // 1/2 medium
		m_medium1.GenerateColorAttachments(1, 1, GL_HALF_FLOAT, 1); // 1/4 medium
		m_medium2.GenerateColorAttachments(1, 1, GL_HALF_FLOAT, 1); // 1/8 medium
		m_medium3.GenerateColorAttachments(1, 1, GL_HALF_FLOAT, 1); // 1/16 medium
		m_origin.GenerateColorAttachments(1, 1, GL_HALF_FLOAT, 1); // origin

		m_filterShader.AttachVertexShader(ReadShaderSource("shaders/effect/effect.quad.vs"));
		m_filterShader.AttachFragmentShader(ReadShaderSource("shaders/effect/effect.bloom.filter.fs"));
		m_filterShader.GenerateAndLink();
		m_filterShader.Bind();
		m_filterShader.SetUniform("TexSrc", 0);
		m_filterShader.Unbind();

		m_blurShader.AttachVertexShader(ReadShaderSource("shaders/effect/effect.quad.vs"));
		m_blurShader.AttachFragmentShader(ReadShaderSource("shaders/effect/effect.bloom.blur.fs"));
		m_blurShader.GenerateAndLink();
		m_blurShader.Bind();
		m_blurShader.SetUniform("TexSrc", 0);
		m_blurShader.Unbind();

		m_postShader.AttachVertexShader(ReadShaderSource("shaders/effect/effect.quad.vs"));
		m_postShader.AttachFragmentShader(ReadShaderSource("shaders/effect/effect.bloom.post.fs"));
		m_postShader.GenerateAndLink();
		m_postShader.Bind();
		m_postShader.SetUniform("TexSrc", 0);
		m_postShader.SetUniform("TexBloom1", 1);
		m_postShader.SetUniform("TexBloom2", 2);
		m_postShader.SetUniform("TexBloom3", 3);
		m_postShader.SetUniform("TexBloom4", 4);
		m_postShader.Unbind();

		m_quad = MeshManager::LoadGlobalPrimitive("quad");
	}

	void BloomRenderer::Resize(unsigned int width, unsigned int height)
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

		m_medium0.Resize(w2, h2);
		m_medium1.Resize(w4, h4);
		m_medium2.Resize(w8, h8);
		m_medium3.Resize(w16, h16);

		m_origin.Resize(w2, h2);
	}

	void BloomRenderer::Generate(const Texture & source)
	{
		m_origin.Bind();
		glViewport(0, 0, m_origin.Width(), m_origin.Height());
		glClear(GL_COLOR_BUFFER_BIT);

		source.Bind(0); // TexSrc

		m_filterShader.Bind();

		RenderMesh(&m_quad);
		bloom_blur_pingpong_op(m_origin.GetColorAttachment(0), m_target0, m_medium0, m_blurShader, m_quad, 8);
		bloom_blur_pingpong_op(m_output0, m_target1, m_medium1, m_blurShader, m_quad, 8);
		bloom_blur_pingpong_op(m_output1, m_target2, m_medium2, m_blurShader, m_quad, 8);
		bloom_blur_pingpong_op(m_output2, m_target3, m_medium3, m_blurShader, m_quad, 8);

		m_filterShader.Unbind();

		m_origin.Unbind(); // also unbind all other frame buffers used in this pass
	}

	void BloomRenderer::Render(const Texture & source)
	{
		source.Bind(0); // TexSrc
		m_output0.Bind(1);
		m_output1.Bind(2);
		m_output2.Bind(3);
		m_output3.Bind(4);

		m_postShader.Bind();

		RenderMesh(&m_quad);

		m_postShader.Unbind();
	}
}