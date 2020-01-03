#include "ssao_renderer.h"

#include <random>
#include <functional> // std::bind

#include <vendor/glad/glad.h>
#include <vendor/glm/glm.hpp>
#include <vendor/glm/gtx/compatibility.hpp>

#include <mesh/mesh_manager.h>

#include "shader_manager.h"
#include "general_renderer.h"

namespace xengine
{
	SSAORenderer::SSAORenderer()
	{
		m_kernelSize = 32; // no bigger than 64 (according to ssao.fs)

		std::uniform_real_distribution<float> random_dist(0.0f, 1.0f);
		std::default_random_engine random_dist_generator;
		auto dice = std::bind(random_dist, random_dist_generator);

		std::vector<glm::vec3> kernel;
		for (int i = 0; i < m_kernelSize; ++i)
		{
			glm::vec3 sample{ dice() * 2.0f - 1.0f, dice() * 2.0f - 1.0f, dice() };
			sample = glm::normalize(sample);
			sample = sample * dice();
			float scale = static_cast<float>(i) / static_cast<float>(m_kernelSize);
			scale = glm::lerp(0.1f, 1.0f, scale * scale);
			sample = sample * scale;
			kernel.push_back(sample);
		}

		std::vector<glm::vec3> noises;
		for (unsigned int i = 0; i < 16; i++)
			noises.push_back({ dice() * 2.0 - 1.0, dice() * 2.0 - 1.0, 0.0f });

		m_shader.AttachVertexShader(ReadShaderSource("shaders/effect/effect.quad.vs"));
		m_shader.AttachFragmentShader(ReadShaderSource("shaders/effect/effect.ssao.capture.fs"));
		m_shader.GenerateAndLink();
		m_shader.Bind();
		m_shader.SetUniform("gPosition", 0);
		m_shader.SetUniform("gNormal", 1);
		m_shader.SetUniform("texNoise", 2);
		m_shader.SetUniform("kernel", static_cast<int>(kernel.size()), kernel);
		m_shader.SetUniform("sampleCount", m_kernelSize);
		m_shader.Unbind();

		m_target.GenerateColorAttachments(1, 1, GL_HALF_FLOAT, 1);

		m_output = m_target.GetColorAttachment(0);

		m_noise.Generate2D(4, 4, GL_RGBA16F, GL_RGB, GL_HALF_FLOAT, &noises[0]);

		m_quad = MeshManager::LoadPrimitive("quad");
	}

	void SSAORenderer::Resize(unsigned int width, unsigned int height)
	{
		m_target.Resize(width / 2, height / 2);

		m_shader.Bind();
		m_shader.SetUniform("renderSize", glm::vec2{ width, height });
		m_shader.Unbind();
	}

	void SSAORenderer::Generate(Texture * gPosition, Texture * gNormal, Camera * camera)
	{
		m_target.Bind();
		glViewport(0, 0, m_target.Width(), m_target.Height());
		glClear(GL_COLOR_BUFFER_BIT);

		gPosition->Bind(0);
		gNormal->Bind(1);
		m_noise.Bind(2);

		m_shader.Bind();
		m_shader.SetUniform("projection", camera->GetProjection());
		m_shader.SetUniform("view", camera->GetView());

		RenderMesh(m_quad);

		m_shader.Unbind();

		m_target.Unbind();
	}
}