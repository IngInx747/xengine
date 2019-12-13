#include "ibl_renderer.h"

#include <vendor/glad/glad.h>
#include <vendor/glm/glm.hpp>

#include <utility/log.h>
#include <geometry/constant.h>
#include <mesh/mesh_manager.h>

#include "../shader/shader_manager.h"
#include "../material/material_manager.h"

namespace xengine
{
	Texture* IblRenderer::_brdfIntegrationMap = nullptr;
	FrameBuffer IblRenderer::_brdfIntegrationMapBuffer;

	IblRenderer::IblRenderer()
	{
		// shaders
		m_environmentCaptureShader = ShaderManager::LoadVF("pbr:environment", "shaders/pbr/pbr.sampler.cube.vs", "shaders/pbr/pbr.environment.fs");
		m_irradianceCaptureShader = ShaderManager::LoadVF("pbr:irradiance", "shaders/pbr/pbr.sampler.cube.vs", "shaders/pbr/pbr.capture.irradiance.fs");
		m_reflectionCaptureShader = ShaderManager::LoadVF("pbr:reflection", "shaders/pbr/pbr.sampler.cube.vs", "shaders/pbr/pbr.capture.reflection.fs");

		// meshes
		m_quad = MeshManager::LoadPrimitive("quad");
		m_cube = MeshManager::LoadPrimitive("cube");
		m_sphere = MeshManager::LoadPrimitive("sphere", 16, 8);

		// captures
		m_environmentCapture.GenerateCubeMap(1024);
		m_environmentCubeMap = m_environmentCapture.captures.GetColorAttachment(0);

		m_irradianceCapture.GenerateCubeMap(32);
		m_envIrradianceCubeMap = m_irradianceCapture.captures.GetColorAttachment(0);

		m_reflectionCapture.GenerateCubeMap(128);
		m_envReflectionCubeMap = m_reflectionCapture.captures.GetColorAttachment(0);
		m_envReflectionCubeMap->SetFilterMin(GL_LINEAR_MIPMAP_LINEAR);
		m_envReflectionCubeMap->EnableMipmap();

		// brdf integration
		generateNormalRoughnessLookup();
	}

	void IblRenderer::GenerateEnvironment(Texture * environment)
	{
		GenerateEnvCubeMap(environment);
		GenerateIrradiance(m_environmentCubeMap);
		GenerateReflection(m_environmentCubeMap);
	}

	void IblRenderer::GenerateEnvCubeMap(Texture * envHdr)
	{
		Material material(m_environmentCaptureShader);
		material.RegisterTexture("environment", envHdr);
		material.attribute.eDepthFunc = GL_LEQUAL;
		material.attribute.bCull = false;

		for (unsigned int i = 0; i < 6; ++i)
		{
			m_environmentCapture.BindFace(i);
			glViewport(0, 0, m_environmentCapture.Width(), m_environmentCapture.Height());
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Camera* camera = &m_irradianceCapture.cameras[i];

			material.shader->Bind();
			material.shader->SetUniform("projection", camera->GetProjection());
			material.shader->SetUniform("view", camera->GetView());
			material.shader->SetUniform("camPos", camera->GetPosition());

			RenderMesh(m_cube, &material);
		}
	}

	void IblRenderer::GenerateIrradiance(CubeMap * capture)
	{
		Material material(m_irradianceCaptureShader);
		material.RegisterTexture("environment", capture);
		material.attribute.eDepthFunc = GL_LEQUAL;
		material.attribute.bCull = false;

		for (unsigned int i = 0; i < 6; ++i)
		{
			m_irradianceCapture.BindFace(i);
			glViewport(0, 0, m_irradianceCapture.Width(), m_irradianceCapture.Height());
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Camera* camera = &m_irradianceCapture.cameras[i];

			material.shader->Bind();
			material.shader->SetUniform("projection", camera->GetProjection());
			material.shader->SetUniform("view", camera->GetView());
			material.shader->SetUniform("camPos", camera->GetPosition());

			RenderMesh(m_cube, &material);
		}

		material.shader->Unbind();
		m_irradianceCapture.Unbind();
	}

	void IblRenderer::GenerateReflection(CubeMap * capture)
	{
		Material material(m_reflectionCaptureShader);
		material.RegisterTexture("environment", capture);
		material.attribute.eDepthFunc = GL_LEQUAL;
		material.attribute.bCull = false;

		for (unsigned int mip = 0; mip < 5; ++mip)
		{
			material.RegisterUniform("roughness", (float)mip / 4.0f);

			unsigned int width = m_reflectionCapture.Width() >> mip;
			unsigned int height = m_reflectionCapture.Height() >> mip;

			for (unsigned int i = 0; i < 6; ++i)
			{
				m_reflectionCapture.BindFace(i, mip);
				glViewport(0, 0, width, height);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				Camera* camera = &m_irradianceCapture.cameras[i];

				material.shader->Bind();
				material.shader->SetUniform("projection", camera->GetProjection());
				material.shader->SetUniform("view", camera->GetView());
				material.shader->SetUniform("camPos", camera->GetPosition());

				RenderMesh(m_cube, &material);
			}
		}
	}

	void IblRenderer::generateNormalRoughnessLookup()
	{
		if (_brdfIntegrationMap) return;

		_brdfIntegrationMapBuffer.GenerateColorAttachments(128, 128, GL_HALF_FLOAT, 1);
		_brdfIntegrationMapBuffer.GenerateDepthRenderBuffer(128, 128);

		_brdfIntegrationMapBuffer.Bind();

		glViewport(0, 0, _brdfIntegrationMapBuffer.Width(), _brdfIntegrationMapBuffer.Height());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Shader *shader = ShaderManager::LoadVF("pbr:integration", "shaders/pbr/pbr.sampler.quad.vs", "shaders/pbr/pbr.brdf_integration.fs");
		Mesh* quad = MeshManager::LoadPrimitive("quad");

		shader->Bind();

		RenderMesh(quad);

		_brdfIntegrationMapBuffer.Unbind();

		_brdfIntegrationMap = _brdfIntegrationMapBuffer.GetColorAttachment(0);
	}
}