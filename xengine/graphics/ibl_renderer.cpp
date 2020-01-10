#include "ibl_renderer.h"

#include <vendor/glad/glad.h>
#include <vendor/glm/glm.hpp>

#include <utility/log.h>
#include <geometry/constant.h>
#include <mesh/mesh_manager.h>

#include "shader_manager.h"
#include "material_manager.h"

namespace xengine
{
	Shader IblRenderer::_environmentCaptureShader; // convert HDR environment 2D texture to environment cubemap
	Shader IblRenderer::_irradianceCaptureShader; // generate the irradiance cubemap from environment cubemap
	Shader IblRenderer::_reflectionCaptureShader; // generate the reflection cubemap from environment cubemap

	Mesh IblRenderer::_quad;
	Mesh IblRenderer::_cube;
	Mesh IblRenderer::_sphere;

	Texture IblRenderer::_brdfIntegrationMap;
	FrameBuffer IblRenderer::_brdfIntegrationMapBuffer;

	void IblRenderer::Initialize()
	{
		// shaders
		_environmentCaptureShader = ShaderManager::LoadGlobalVF("pbr:environment", "shaders/pbr/pbr.sampler.cube.vs", "shaders/pbr/pbr.environment.fs");
		_irradianceCaptureShader = ShaderManager::LoadGlobalVF("pbr:irradiance", "shaders/pbr/pbr.sampler.cube.vs", "shaders/pbr/pbr.capture.irradiance.fs");
		_reflectionCaptureShader = ShaderManager::LoadGlobalVF("pbr:reflection", "shaders/pbr/pbr.sampler.cube.vs", "shaders/pbr/pbr.capture.reflection.fs");

		// meshes
		_quad = MeshManager::LoadGlobalPrimitive("quad");
		_cube = MeshManager::LoadGlobalPrimitive("cube");
		_sphere = MeshManager::LoadGlobalPrimitive("sphere", 16, 8);

		// brdf integration
		generateNormalRoughnessLookup();
	}

	FrameBuffer IblRenderer::CreateEnvironment(const Texture& environment)
	{
		Material material(_environmentCaptureShader);
		material.RegisterTexture("environment", environment);
		material.attribute.eDepthFunc = GL_LEQUAL;
		material.attribute.bCull = false;

		CubicCapture capture;
		capture.GenerateCubeMap(1024);

		for (unsigned int i = 0; i < 6; ++i)
		{
			capture.BindFace(i);
			glViewport(0, 0, capture.Width(), capture.Height());
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Camera* camera = &capture.cameras[i];

			material.shader.Bind();
			material.shader.SetUniform("projection", camera->GetProjection());
			material.shader.SetUniform("view", camera->GetView());
			material.shader.SetUniform("camPos", camera->GetPosition());

			RenderMesh(&_cube, &material);
		}

		return capture.captures;
	}

	FrameBuffer IblRenderer::CreateIrradiance(const CubeMap& environment)
	{
		Material material(_irradianceCaptureShader);
		material.RegisterTexture("environment", environment);
		material.attribute.eDepthFunc = GL_LEQUAL;
		material.attribute.bCull = false;

		CubicCapture capture;
		capture.GenerateCubeMap(32);

		for (unsigned int i = 0; i < 6; ++i)
		{
			capture.BindFace(i);
			glViewport(0, 0, capture.Width(), capture.Height());
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Camera* camera = &capture.cameras[i];

			material.shader.Bind();
			material.shader.SetUniform("projection", camera->GetProjection());
			material.shader.SetUniform("view", camera->GetView());
			material.shader.SetUniform("camPos", camera->GetPosition());

			RenderMesh(&_cube, &material);
		}

		return capture.captures;
	}

	FrameBuffer IblRenderer::CreateReflection(const CubeMap& environment)
	{
		Material material(_reflectionCaptureShader);
		material.RegisterTexture("environment", environment);
		material.attribute.eDepthFunc = GL_LEQUAL;
		material.attribute.bCull = false;

		CubicCapture capture;
		capture.GenerateCubeMap(128);

		CubeMap & cubeMap = capture.captures.GetColorAttachment(0);
		cubeMap.SetFilterMin(GL_LINEAR_MIPMAP_LINEAR);
		cubeMap.SetMipmap(true);

		for (unsigned int mip = 0; mip < 5; ++mip)
		{
			material.RegisterUniform("roughness", (float)mip / 4.0f);

			unsigned int width = capture.Width() >> mip;
			unsigned int height = capture.Height() >> mip;

			for (unsigned int i = 0; i < 6; ++i)
			{
				capture.BindFace(i, mip);
				glViewport(0, 0, width, height);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				Camera* camera = &capture.cameras[i];

				material.shader.Bind();
				material.shader.SetUniform("projection", camera->GetProjection());
				material.shader.SetUniform("view", camera->GetView());
				material.shader.SetUniform("camPos", camera->GetPosition());

				RenderMesh(&_cube, &material);
			}
		}

		return capture.captures;
	}

	void IblRenderer::generateNormalRoughnessLookup()
	{
		_brdfIntegrationMapBuffer.GenerateColorAttachments(128, 128, GL_HALF_FLOAT, 1);
		_brdfIntegrationMapBuffer.GenerateDepthRenderBuffer(128, 128);

		_brdfIntegrationMapBuffer.Bind();

		glViewport(0, 0, _brdfIntegrationMapBuffer.Width(), _brdfIntegrationMapBuffer.Height());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Shader shader = ShaderManager::LoadGlobalVF("pbr:integration", "shaders/pbr/pbr.sampler.quad.vs", "shaders/pbr/pbr.brdf_integration.fs");

		shader.Bind();

		RenderMesh(&_quad);

		_brdfIntegrationMapBuffer.Unbind();

		_brdfIntegrationMap = _brdfIntegrationMapBuffer.GetColorAttachment(0);
	}
}