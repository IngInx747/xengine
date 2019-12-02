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
		m_gBuffer.GenerateColorAttachments(1, 1, GL_HALF_FLOAT, 4);

		// set color attachments properly
		m_gBuffer.BindColorAttachment(0, 0);
		m_gBuffer.BindColorAttachment(1, 1);
		m_gBuffer.BindColorAttachment(2, 2);
		m_gBuffer.BindColorAttachment(3, 3);

		// geometry buffer requires 1 depth attachment for 3d scene depth test
		//m_gBuffer.GenerateDepthAttachment(1, 1, GL_HALF_FLOAT);

		// actually depth value will not be used except depth test, so an RBO is a better choice
		m_gBuffer.GenerateDepthRenderBuffer(1, 1);

		m_parallelLightShader = ShaderManager::LoadVertFragShader("deferred directional", "shaders/deferred/screen_directional.vs", "shaders/deferred/directional.fs");
		m_pointLightShader = ShaderManager::LoadVertFragShader("deferred point", "shaders/deferred/point.vs", "shaders/deferred/point.fs");
		m_ambientLightShader = ShaderManager::LoadVertFragShader("deferred ambient", "shaders/deferred/screen_ambient.vs", "shaders/deferred/ambient.fs");

		m_quad = MeshManager::LoadPrimitive("quad");
		m_sphere = MeshManager::LoadPrimitive("sphere", 16, 8);
	}

	void DeferredRenderer::Resize(unsigned int width, unsigned int height)
	{
		m_gBuffer.Resize(width, height);
	}

	void DeferredRenderer::GenerateGeometry(const std::vector<RenderCommand>& commands)
	{
		m_gBuffer.Bind();

		// enable usage of 4 texture attachments of framebuffer
		unsigned int attachments[4] = {
			GL_COLOR_ATTACHMENT0,
			GL_COLOR_ATTACHMENT1,
			GL_COLOR_ATTACHMENT2,
			GL_COLOR_ATTACHMENT3
		};
		glDrawBuffers(4, attachments);

		glViewport(0, 0, m_gBuffer.Width(), m_gBuffer.Height());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		OglStatus::Lock(); // we don't want materials change OpenGL settings in this pass
		{
			for (const RenderCommand& command : commands)
				GeneralRenderer::RenderSingleCommand(command);
		}
		OglStatus::Unlock();

		// disable usage of attachments
		attachments[1] = GL_NONE;
		attachments[2] = GL_NONE;
		attachments[3] = GL_NONE;
		glDrawBuffers(4, attachments);

		m_gBuffer.Unbind();
	}

	void DeferredRenderer::RenderParallelLights(const std::vector<ParallelLight*>& lights, Camera * camera)
	{
		GetTexPosition()->Bind(0); // gPositionMetallic
		GetTexNormal()->Bind(1); // gNormalRoughness
		GetTexAlbedo()->Bind(2); // gAlbedoAO

		Shader* shader = m_parallelLightShader;

		shader->Use();
		shader->SetUniform("gPositionMetallic", 0);
		shader->SetUniform("gNormalRoughness", 1);
		shader->SetUniform("gAlbedoAO", 2);
		shader->SetUniform("lightShadowMap", 3);
		shader->SetUniform("ShadowsEnabled", RenderConfig::UseShadow());

		OglStatus::SetDepthTest(GL_FALSE);
		OglStatus::SetBlend(GL_TRUE);
		OglStatus::SetBlendFunc(GL_ONE, GL_ONE);

		for (ParallelLight* light : lights)
		{
			ParallelShadow& shadow = light->shadow;
			shadow.GetFrameBuffer()->GetDepthStencilAttachment(0)->Bind(3); // lightShadowMap

			shader->SetUniform("lightDir", light->direction);
			shader->SetUniform("lightColor", glm::normalize(light->color) * light->intensity);
			shader->SetUniform("lightShadowViewProjection", shadow.GetViewProj());

			GeneralRenderer::RenderMesh(m_quad);
		}

		OglStatus::SetBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		OglStatus::SetBlend(GL_FALSE);
		OglStatus::SetDepthTest(GL_TRUE);
	}

	void DeferredRenderer::RenderPointLights(const std::vector<PointLight*>& lights, Camera * camera)
	{
		GetTexPosition()->Bind(0); // gPositionMetallic
		GetTexNormal()->Bind(1); // gNormalRoughness
		GetTexAlbedo()->Bind(2); // gAlbedoAO

		Shader* shader = m_pointLightShader;

		shader->Use();
		shader->SetUniform("gPositionMetallic", 0);
		shader->SetUniform("gNormalRoughness", 1);
		shader->SetUniform("gAlbedoAO", 2);

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

			GeneralRenderer::RenderMesh(m_sphere);
		}

		OglStatus::SetCullFace(GL_BACK);

		OglStatus::SetBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		OglStatus::SetBlend(GL_FALSE);
		OglStatus::SetDepthTest(GL_TRUE);
	}

	void DeferredRenderer::RenderAmbientLight(CubeMap * irradiance, CubeMap * reflection, Texture * ambientOcclusion)
	{
		GetTexPosition()->Bind(0); // gPositionMetallic
		GetTexNormal()->Bind(1); // gNormalRoughness
		GetTexAlbedo()->Bind(2); // gAlbedoAO
		irradiance->Bind(3); // envIrradiance
		reflection->Bind(4); // envReflection
		IblRenderer::GetNormalRoughnessLookup()->Bind(5);
		ambientOcclusion->Bind(6);

		Shader* shader = m_ambientLightShader;

		shader->Use();
		shader->SetUniform("gPositionMetallic", 0);
		shader->SetUniform("gNormalRoughness", 1);
		shader->SetUniform("gAlbedoAO", 2);
		shader->SetUniform("envIrradiance", 3);
		shader->SetUniform("envReflection", 4);
		shader->SetUniform("BRDFLUT", 5);
		shader->SetUniform("TexSSAO", 6);
		shader->SetUniform("SSAO", RenderConfig::UseSSAO());

		GeneralRenderer::RenderMesh(m_quad);
	}
}