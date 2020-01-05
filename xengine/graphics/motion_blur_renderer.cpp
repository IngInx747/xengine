#include "motion_blur_renderer.h"

#include <vendor/glad/glad.h>

#include <mesh/mesh_manager.h>

#include "ogl_status.h"
#include "general_renderer.h"

namespace xengine
{
	MotionBlurRenderer::MotionBlurRenderer()
	{
		m_target.GenerateColorAttachments(1, 1, GL_HALF_FLOAT, 1);

		m_captureShader.AttachVertexShader(ReadShaderSource("shaders/effect/effect.quad.vs"));
		m_captureShader.AttachFragmentShader(ReadShaderSource("shaders/effect/effect.motion_blur.capture.fs"));
		m_captureShader.GenerateAndLink();
		m_captureShader.Bind();
		m_captureShader.SetUniform("gPosition", 0);
		m_captureShader.Unbind();

		m_blitShader.AttachVertexShader(ReadShaderSource("shaders/effect/effect.quad.vs"));
		m_blitShader.AttachFragmentShader(ReadShaderSource("shaders/effect/effect.motion_blur.blit.fs"));
		m_blitShader.GenerateAndLink();
		m_blitShader.Bind();
		m_blitShader.SetUniform("TexSrc", 0);
		m_blitShader.Unbind();

		m_postShader.AttachVertexShader(ReadShaderSource("shaders/effect/effect.quad.vs"));
		m_postShader.AttachFragmentShader(ReadShaderSource("shaders/effect/effect.motion_blur.post.fs"));
		m_postShader.GenerateAndLink();
		m_postShader.Bind();
		m_postShader.SetUniform("TexSrc", 0);
		m_postShader.SetUniform("TexMotion", 1);
		m_postShader.SetUniform("MotionScale", 1.0f);
		m_postShader.SetUniform("MotionSamples", 16);
		m_postShader.Unbind();

		m_quad = MeshManager::LoadPrimitive("quad");
	}

	void MotionBlurRenderer::Resize(unsigned int width, unsigned int height)
	{
		m_target.Resize(width, height);
	}

	void MotionBlurRenderer::Generate(const Texture & gPosition, Camera* camera)
	{
		m_target.Bind();
		glViewport(0, 0, m_target.Width(), m_target.Height());
		glClear(GL_COLOR_BUFFER_BIT);

		gPosition.Bind(0); // TexSrc

		m_captureShader.Bind();
		m_captureShader.SetUniform("camPos", camera->GetPosition());
		m_captureShader.SetUniform("camForward", camera->GetForward());
		m_captureShader.SetUniform("camUp", camera->GetUp());
		m_captureShader.SetUniform("camRight", camera->GetRight());
		m_captureShader.SetUniform("currView", camera->GetView());
		m_captureShader.SetUniform("prevView", camera->GetPrevView());
		m_captureShader.SetUniform("projection", camera->GetProjection());

		RenderMesh(m_quad);

		m_captureShader.Unbind();

		m_target.Unbind();
	}

	void MotionBlurRenderer::Render(const Texture & source)
	{
		source.Bind(0); // TexSrc
		m_target.GetColorAttachment(0).Bind(1); // TexMotion

		m_postShader.Bind();

		RenderMesh(m_quad);

		m_postShader.Unbind();
	}

	void MotionBlurRenderer::AttachMotion(const Texture & source)
	{
		m_target.Bind();
		glViewport(0, 0, m_target.Width(), m_target.Height());

		OglStatus::SetDepthTest(GL_FALSE);
		OglStatus::SetBlend(GL_TRUE);
		OglStatus::SetBlendFunc(GL_ONE, GL_ONE);

		source.Bind(0); // TexSrc

		m_blitShader.Bind();

		RenderMesh(m_quad);

		m_blitShader.Unbind();

		OglStatus::SetBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		OglStatus::SetBlend(GL_FALSE);
		OglStatus::SetDepthTest(GL_TRUE);
	}
}