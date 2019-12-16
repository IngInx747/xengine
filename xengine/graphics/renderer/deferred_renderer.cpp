#include "deferred_renderer.h"

#include <vendor/glad/glad.h>
#include <vendor/glm/glm.hpp>
#include <vendor/glm/gtc/matrix_transform.hpp>

#include <mesh/mesh_manager.h>

#include "../platform/ogl_status.h"
#include "../shader/shader_manager.h"

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

		m_ambientLightShader = ShaderManager::LoadVF("deferred ambient", "shaders/deferred/deferred.quad.vs", "shaders/deferred/deferred.lighting.ambient.fs");
		m_ambientLightShader->Bind();
		m_ambientLightShader->SetUniform("gPosition", 0);
		m_ambientLightShader->SetUniform("gNormal", 1);
		m_ambientLightShader->SetUniform("gAlbedo", 2);
		m_ambientLightShader->SetUniform("gPbrParam", 3);
		m_ambientLightShader->SetUniform("envIrradiance", 4);
		m_ambientLightShader->SetUniform("envReflection", 5);
		m_ambientLightShader->SetUniform("BRDFLUT", 6);
		m_ambientLightShader->SetUniform("TexSSAO", 7);
		m_ambientLightShader->Unbind();

		m_parallelLightShader = ShaderManager::LoadVF("deferred parallel", "shaders/deferred/deferred.quad.vs", "shaders/deferred/deferred.lighting.parallel.fs");
		m_parallelLightShader->Bind();
		m_parallelLightShader->SetUniform("gPosition", 0);
		m_parallelLightShader->SetUniform("gNormal", 1);
		m_parallelLightShader->SetUniform("gAlbedo", 2);
		m_parallelLightShader->SetUniform("gPbrParam", 3);
		m_parallelLightShader->SetUniform("TexSSAO", 4);
		m_parallelLightShader->SetUniform("lightShadowMap", 5);
		m_parallelLightShader->Unbind();

		m_pointLightShader = ShaderManager::LoadVF("deferred point", "shaders/deferred/deferred.sphere.vs", "shaders/deferred/deferred.lighting.point.fs");
		m_pointLightShader->Bind();
		m_pointLightShader->SetUniform("gPosition", 0);
		m_pointLightShader->SetUniform("gNormal", 1);
		m_pointLightShader->SetUniform("gAlbedo", 2);
		m_pointLightShader->SetUniform("gPbrParam", 3);
		m_pointLightShader->Unbind();

		m_quad = MeshManager::LoadPrimitive("quad");
		m_sphere = MeshManager::LoadPrimitive("sphere", 16, 8);
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
				Material* material = command.material;
				Mesh* mesh = command.mesh;

				material->shader->Bind();
				material->shader->SetUniform("model", command.transform);
				material->shader->SetUniform("prevModel", command.prevTrans);

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

	void DeferredRenderer::RenderParallelLights(const std::vector<ParallelLight*>& lights, Camera * camera, Texture * ao)
	{
		GetTexPosition()->Bind(0); // gPosition
		GetTexNormal()->Bind(1); // gNormal
		GetTexAlbedo()->Bind(2); // gAlbedo
		GetTexPbrParam()->Bind(3); // gPbrParam
		ao->Bind(4); // TexSSAO

		Shader* shader = m_parallelLightShader;

		shader->Bind();
		shader->SetUniform("UseParallelShadow", RenderConfig::UseParallelShadow());
		shader->SetUniform("UseSSAO", RenderConfig::UseSSAO());

		OglStatus::SetDepthTest(GL_FALSE);
		OglStatus::SetBlend(GL_TRUE);
		OglStatus::SetBlendFunc(GL_ONE, GL_ONE);

		for (ParallelLight* light : lights)
		{
			ParallelShadow& shadow = light->shadow;
			shadow.GetFrameBuffer()->GetDepthStencilAttachment(0)->Bind(5); // lightShadowMap

			shader->SetUniform("lightDir", light->direction);
			shader->SetUniform("lightColor", glm::normalize(light->color) * light->intensity);
			shader->SetUniform("lightShadowViewProjection", shadow.GetViewProj());

			RenderMesh(m_quad);
		}

		OglStatus::SetBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		OglStatus::SetBlend(GL_FALSE);
		OglStatus::SetDepthTest(GL_TRUE);

		shader->Unbind();
	}

	void DeferredRenderer::RenderPointLights(const std::vector<PointLight*>& lights, Camera * camera)
	{
		GetTexPosition()->Bind(0); // gPositionMetallic
		GetTexNormal()->Bind(1); // gNormalRoughness
		GetTexAlbedo()->Bind(2); // gAlbedoAO
		GetTexPbrParam()->Bind(3); // gPbrParam

		Shader* shader = m_pointLightShader;

		shader->Bind();

		OglStatus::SetDepthTest(GL_FALSE);
		OglStatus::SetBlend(GL_TRUE);
		OglStatus::SetBlendFunc(GL_ONE, GL_ONE);

		// Note: We only render the back hemisphere, instead of the front one.
		// If we render the front one, lighting will fail when observer enters light volumn.
		OglStatus::SetCullFace(GL_FRONT);

		for (PointLight* light : lights)
		{
			if (!camera->IntersectFrustum(light->position, light->radius)) continue;

			glm::mat4 model;
			glm::mat4 translate = glm::translate(glm::mat4{}, light->position);
			glm::mat4 scale = glm::scale(glm::mat4{}, glm::vec3(light->radius));
			model = translate * scale;

			shader->SetUniform("model", model);
			shader->SetUniform("lightPos", light->position);
			shader->SetUniform("lightRadius", light->radius);
			shader->SetUniform("lightColor", glm::normalize(light->color) * light->intensity);

			RenderMesh(m_sphere);
		}

		OglStatus::SetCullFace(GL_BACK);

		OglStatus::SetBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		OglStatus::SetBlend(GL_FALSE);
		OglStatus::SetDepthTest(GL_TRUE);

		shader->Unbind();
	}

	void DeferredRenderer::RenderAmbientLight(CubeMap * irradiance, CubeMap * reflection, Texture * ao)
	{
		GetTexPosition()->Bind(0); // gPositionMetallic
		GetTexNormal()->Bind(1); // gNormalRoughness
		GetTexAlbedo()->Bind(2); // gAlbedoAO
		GetTexPbrParam()->Bind(3); // gPbrParam
		if (irradiance) irradiance->Bind(4); // envIrradiance
		if (reflection) reflection->Bind(5); // envReflection
		IblRenderer::GetBrdfIntegrationMap()->Bind(6); // BRDFLUT
		ao->Bind(7); // TexSSAO

		Shader* shader = m_ambientLightShader;

		shader->Bind();
		shader->SetUniform("UseSSAO", RenderConfig::UseSSAO());
		shader->SetUniform("UseSSR", RenderConfig::UseSSR());

		RenderMesh(m_quad);

		shader->Unbind();
	}
}