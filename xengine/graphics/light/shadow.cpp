#include "shadow.h"

#include <vendor/glad/glad.h>
#include <vendor/glm/glm.hpp>
#include <vendor/glm/gtc/matrix_transform.hpp>

#include <geometry/constant.h>

namespace xengine
{
	////////////////////////////////////////////////////////////////
	// Shadow
	////////////////////////////////////////////////////////////////

	void Shadow::Resize(unsigned int width, unsigned int height)
	{
		m_shadowMap.Resize(width, height);
	}

	////////////////////////////////////////////////////////////////
	// Shadow: Parallel Shadow
	////////////////////////////////////////////////////////////////

	ParallelShadow::ParallelShadow()
	{
		// shadow affecting volumn
		camera.SetProjOrtho(-20.0f, 20.0f, -20.0f, 20.0f, -15.0f, 20.0f);

		// default shadow casting direction and center
		UpdateView({ 1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
	}

	void ParallelShadow::GenerateShadowMap(unsigned int width, unsigned height)
	{
		// generate depth attachment(s)
		// alternative data type can be used: GL_FLOAT, GL_DEPTH_COMPONENT24
		m_shadowMap.GenerateDepthAttachment(width, height, GL_UNSIGNED_BYTE, 1);

		// bind depth attachment(s) for read/write
		m_shadowMap.BindDepthAttachment(0);

		// a depth-stencil attachment also meets shadow value reading requirement (alternative)
		//m_shadowMap.GenerateDepthStencilAttachment(width, height);

		// set shadow value on the border (optional)
		SetBorderDepth(true);

		// set no color will be written to the shadow frame buffer (make sure it runs a depth-only pass) (optional)
		m_shadowMap.Bind();
		{
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}
		m_shadowMap.Unbind();
	}

	void ParallelShadow::UpdateView(const glm::vec3 & lightDir, const glm::vec3 & center)
	{
		glm::vec3 eye = -lightDir * 10.0f + center;

		glm::vec3 up{ 0, 1, 0 };
		if (glm::length(glm::cross(up, eye)) < kEps)
			up = glm::vec3(0, 0, -1);

		camera.SetView(eye, center, up);
		viewProj = camera.GetProjection() * camera.GetView();
	}

	void ParallelShadow::SetBorderDepth(bool flag)
	{
		if (!m_shadowMap.GetDepthStencilAttachment(0)) return;

		m_shadowMap.GetDepthStencilAttachment(0)->SetWrapSTR(GL_CLAMP_TO_BORDER);

		m_shadowMap.GetDepthStencilAttachment(0)->Bind();
		{
			float depth = flag ? 1.0f : 0.0f;
			float borderColor[] = { depth, depth, depth, depth };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		}
		m_shadowMap.GetDepthStencilAttachment(0)->Unbind();
	}
}