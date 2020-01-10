#include "deferred_renderer.h"

#include <vendor/glad/glad.h>
#include <vendor/glm/glm.hpp>
#include <vendor/glm/gtc/matrix_transform.hpp>

#include <mesh/mesh_manager.h>

#include "ogl_status.h"
#include "shader_manager.h"

#include "render_config.h"
#include "general_renderer.h"
#include "ibl_renderer.h"

namespace xengine
{
	DeferredRenderer::DeferredRenderer()
	{
		// geometry buffer requires 4 color attachments (PositionMetallic, NormalRoughness, AlbedoAO, Motion)
		m_gBuffer.GenerateColorAttachments(1, 1, GL_HALF_FLOAT, 5);

		// set color attachments properly
		m_gBuffer.BindColorAttachment(0, 0);
		m_gBuffer.BindColorAttachment(1, 1);
		m_gBuffer.BindColorAttachment(2, 2);
		m_gBuffer.BindColorAttachment(3, 3);
		m_gBuffer.BindColorAttachment(4, 4);

		// geometry buffer requires 1 depth attachment for 3d scene depth test
		//m_gBuffer.GenerateDepthAttachment(1, 1, GL_HALF_FLOAT);

		// actually depth value will not be used except depth test, so an RBO is a better choice
		m_gBuffer.GenerateDepthRenderBuffer(1, 1);

		m_ambientLightShader.AttachVertexShader(ReadShaderSource("shaders/deferred/deferred.quad.vs"));
		m_ambientLightShader.AttachFragmentShader(ReadShaderSource("shaders/deferred/deferred.lighting.ambient.fs"));
		m_ambientLightShader.GenerateAndLink();
		m_ambientLightShader.Bind();
		m_ambientLightShader.SetUniform("gPosition", 0);
		m_ambientLightShader.SetUniform("gNormal", 1);
		m_ambientLightShader.SetUniform("gAlbedo", 2);
		m_ambientLightShader.SetUniform("gPbrParam", 3);
		m_ambientLightShader.SetUniform("envIrradiance", 4);
		m_ambientLightShader.SetUniform("envReflection", 5);
		m_ambientLightShader.SetUniform("BRDFLUT", 6);
		m_ambientLightShader.SetUniform("TexSSAO", 7);
		m_ambientLightShader.Unbind();

		m_parallelLightShader.AttachVertexShader(ReadShaderSource("shaders/deferred/deferred.quad.vs"));
		m_parallelLightShader.AttachFragmentShader(ReadShaderSource("shaders/deferred/deferred.lighting.parallel.fs"));
		m_parallelLightShader.GenerateAndLink();
		m_parallelLightShader.Bind();
		m_parallelLightShader.SetUniform("gPosition", 0);
		m_parallelLightShader.SetUniform("gNormal", 1);
		m_parallelLightShader.SetUniform("gAlbedo", 2);
		m_parallelLightShader.SetUniform("gPbrParam", 3);
		m_parallelLightShader.SetUniform("TexSSAO", 4);
		m_parallelLightShader.SetUniform("lightShadowMap", 5);
		m_parallelLightShader.Unbind();

		m_pointLightShader.AttachVertexShader(ReadShaderSource("shaders/deferred/deferred.sphere.vs"));
		m_pointLightShader.AttachFragmentShader(ReadShaderSource("shaders/deferred/deferred.lighting.point.fs"));
		m_pointLightShader.GenerateAndLink();
		m_pointLightShader.Bind();
		m_pointLightShader.SetUniform("gPosition", 0);
		m_pointLightShader.SetUniform("gNormal", 1);
		m_pointLightShader.SetUniform("gAlbedo", 2);
		m_pointLightShader.SetUniform("gPbrParam", 3);
		m_pointLightShader.Unbind();

		m_reflectLightShader.AttachVertexShader(ReadShaderSource("shaders/deferred/deferred.quad.vs"));
		m_reflectLightShader.AttachFragmentShader(ReadShaderSource("shaders/deferred/deferred.lighting.reflect.fs"));
		m_reflectLightShader.GenerateAndLink();
		m_reflectLightShader.Bind();
		m_reflectLightShader.SetUniform("gPosition", 0);
		m_reflectLightShader.SetUniform("gNormal", 1);
		m_reflectLightShader.SetUniform("gPbrParam", 2);
		m_reflectLightShader.SetUniform("LastImage", 3);
		m_reflectLightShader.Unbind();

		m_quad = MeshManager::LoadGlobalPrimitive("quad");
		m_sphere = MeshManager::LoadGlobalPrimitive("sphere", 16, 8);
	}

	void DeferredRenderer::Resize(unsigned int width, unsigned int height)
	{
		m_gBuffer.Resize(width, height);
	}

	void DeferredRenderer::Generate(const std::vector<RenderCommand>& commands)
	{
		m_gBuffer.Bind();

		// enable usage of 4 texture attachments of framebuffer
		unsigned int attachments[5] = {
			GL_COLOR_ATTACHMENT0,
			GL_COLOR_ATTACHMENT1,
			GL_COLOR_ATTACHMENT2,
			GL_COLOR_ATTACHMENT3,
			GL_COLOR_ATTACHMENT4,
		};
		glDrawBuffers(5, attachments);

		glViewport(0, 0, m_gBuffer.Width(), m_gBuffer.Height());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		OglStatus::Lock(); // we don't want materials change OpenGL settings in this pass
		{
			for (const RenderCommand& command : commands)
			{
				Material * material = command.material;
				Mesh * mesh = command.mesh;

				material->shader.Bind();
				material->shader.SetUniform("model", command.transform);
				material->shader.SetUniform("prevModel", command.prevTrans);

				RenderMesh(mesh, material);
			}
		}
		OglStatus::Unlock();

		// disable usage of attachments
		attachments[1] = GL_NONE;
		attachments[2] = GL_NONE;
		attachments[3] = GL_NONE;
		attachments[4] = GL_NONE;
		glDrawBuffers(5, attachments);

		m_gBuffer.Unbind();
	}

	void DeferredRenderer::RenderParallelLights(const std::vector<ParallelLight*>& lights, Camera * camera, const Texture & ao)
	{
		GetTexPosition().Bind(0); // gPosition
		GetTexNormal().Bind(1); // gNormal
		GetTexAlbedo().Bind(2); // gAlbedo
		GetTexPbrParam().Bind(3); // gPbrParam
		ao.Bind(4); // TexSSAO

		OglStatus::SetDepthTest(GL_FALSE);
		OglStatus::SetBlend(GL_TRUE);
		OglStatus::SetBlendFunc(GL_ONE, GL_ONE);

		m_parallelLightShader.Bind();
		m_parallelLightShader.SetUniform("UseParallelShadow", RenderConfig::UseParallelShadow());
		m_parallelLightShader.SetUniform("UseSSAO", RenderConfig::UseSSAO());

		for (ParallelLight* light : lights)
		{
			ParallelShadow& shadow = light->shadow;
			shadow.GetFrameBuffer()->GetDepthStencilAttachment(0).Bind(5); // lightShadowMap

			m_parallelLightShader.SetUniform("lightDir", light->direction);
			m_parallelLightShader.SetUniform("lightColor", glm::normalize(light->color) * light->intensity);
			m_parallelLightShader.SetUniform("lightShadowViewProjection", shadow.GetViewProj());

			RenderMesh(&m_quad);
		}

		m_parallelLightShader.Unbind();

		OglStatus::SetBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		OglStatus::SetBlend(GL_FALSE);
		OglStatus::SetDepthTest(GL_TRUE);
	}

	void DeferredRenderer::RenderPointLights(const std::vector<PointLight*>& lights, Camera * camera)
	{
		GetTexPosition().Bind(0); // gPositionMetallic
		GetTexNormal().Bind(1); // gNormalRoughness
		GetTexAlbedo().Bind(2); // gAlbedoAO
		GetTexPbrParam().Bind(3); // gPbrParam

		OglStatus::SetDepthTest(GL_FALSE);
		OglStatus::SetBlend(GL_TRUE);
		OglStatus::SetBlendFunc(GL_ONE, GL_ONE);

		// Note: We only render the back hemisphere, instead of the front one.
		// If we render the front one, lighting will fail when observer enters light volumn.
		OglStatus::SetCullFace(GL_FRONT);

		m_pointLightShader.Bind();

		for (PointLight* light : lights)
		{
			if (!camera->IntersectFrustum(light->position, light->radius)) continue;

			glm::mat4 model;
			glm::mat4 translate = glm::translate(glm::mat4{}, light->position);
			glm::mat4 scale = glm::scale(glm::mat4{}, glm::vec3(light->radius));
			model = translate * scale;

			m_pointLightShader.SetUniform("model", model);
			m_pointLightShader.SetUniform("lightPos", light->position);
			m_pointLightShader.SetUniform("lightRadius", light->radius);
			m_pointLightShader.SetUniform("lightColor", glm::normalize(light->color) * light->intensity);

			RenderMesh(&m_sphere);
		}

		m_pointLightShader.Unbind();

		OglStatus::SetCullFace(GL_BACK);

		OglStatus::SetBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		OglStatus::SetBlend(GL_FALSE);
		OglStatus::SetDepthTest(GL_TRUE);
	}

	void DeferredRenderer::RenderAmbientLight(const CubeMap & irradiance, const CubeMap & reflection, const Texture & ao)
	{
		GetTexPosition().Bind(0); // gPositionMetallic
		GetTexNormal().Bind(1); // gNormalRoughness
		GetTexAlbedo().Bind(2); // gAlbedoAO
		GetTexPbrParam().Bind(3); // gPbrParam
		if (irradiance) irradiance.Bind(4); // envIrradiance
		if (reflection) reflection.Bind(5); // envReflection
		IblRenderer::GetBrdfIntegrationMap().Bind(6); // BRDFLUT
		ao.Bind(7); // TexSSAO

		OglStatus::SetDepthTest(GL_FALSE);
		OglStatus::SetBlend(GL_TRUE);
		OglStatus::SetBlendFunc(GL_ONE, GL_ONE);

		m_ambientLightShader.Bind();
		m_ambientLightShader.SetUniform("UseSSAO", RenderConfig::UseSSAO());

		RenderMesh(&m_quad);

		m_ambientLightShader.Unbind();

		OglStatus::SetBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		OglStatus::SetBlend(GL_FALSE);
		OglStatus::SetDepthTest(GL_TRUE);
	}

	void DeferredRenderer::RenderReflectLight(const Texture & last_frame)
	{
		GetTexPosition().Bind(0); // gPositionMetallic
		GetTexNormal().Bind(1); // gNormalRoughness
		GetTexPbrParam().Bind(2); // gPbrParam
		last_frame.Bind(3); // LastImage

		OglStatus::SetDepthTest(GL_FALSE);
		OglStatus::SetBlend(GL_TRUE);
		OglStatus::SetBlendFunc(GL_ONE, GL_ONE);

		m_reflectLightShader.Bind();

		RenderMesh(&m_quad);

		m_reflectLightShader.Unbind();

		OglStatus::SetBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		OglStatus::SetBlend(GL_FALSE);
		OglStatus::SetDepthTest(GL_TRUE);
	}
}