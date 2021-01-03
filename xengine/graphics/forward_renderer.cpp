#include "forward_renderer.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <mesh/mesh.h>
#include <mesh/mesh_manager.h>

#include "shader_manager.h"
#include "ogl_status.h"
#include "render_config.h"
#include "general_renderer.h"

namespace xengine
{
	ForwardRenderer::ForwardRenderer()
	{
		m_parallelShadowShader.AttachVertexShader(ReadShaderSource("shaders/shadow_cast.vs"));
		m_parallelShadowShader.AttachFragmentShader(ReadShaderSource("shaders/shadow_cast.fs"));
		m_parallelShadowShader.GenerateAndLink();

		m_volumnLightShader.AttachVertexShader(ReadShaderSource("shaders/light.vs"));
		m_volumnLightShader.AttachFragmentShader(ReadShaderSource("shaders/light.fs"));
		m_volumnLightShader.GenerateAndLink();

		m_sphere = MeshManager::LoadGlobalPrimitive("sphere", 16, 8);
	}

	void ForwardRenderer::GenerateParallelShadow(const std::vector<RenderCommand>& commands, const std::vector<ParallelLight*>& lights, Camera* camera)
	{
		OglStatus::SetCullFace(GL_FRONT); // no need to render front-facing triangles

		for (ParallelLight* light : lights)
		{
			if (!light->useShadowCast) continue;

			// uncomment after implementation of cascaded shadow map
			// before that shadow view is set at (0, 0, 0)
			//glm::vec3 camPos = camera->GetPosition();
			//camPos.y = 0.0f;
			//light->UpdateShadowView(camPos); // let light follow the camera

			ParallelShadow& shadow = light->shadow;
			shadow.GetFrameBuffer()->Bind();
			glViewport(0, 0, shadow.GetFrameBuffer()->Width(), shadow.GetFrameBuffer()->Height());
			glClear(GL_DEPTH_BUFFER_BIT); // each light's framebuffer need be refreshed per frame

			m_parallelShadowShader.Bind();
			m_parallelShadowShader.SetUniform("projection", shadow.GetProj());
			m_parallelShadowShader.SetUniform("view", shadow.GetView());

			// we only care about depth info so we don't use RenderCommand(...) which is more expensive
			for (const RenderCommand& command : commands)
			{
				if (!light->shadow.GetCamera()->IntersectFrustum(command.aabb)) continue;

				m_parallelShadowShader.SetUniform("model", command.transform);
				RenderMesh(command.mesh);
			}
		}

		OglStatus::SetCullFace(GL_BACK); // restore original culling setting

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void ForwardRenderer::SetParallelShadow(const std::vector<ParallelLight*>& lights, const std::vector<RenderCommand>& commands)
	{
		std::unordered_set<Shader*> shaders;

		for (const RenderCommand& command : commands)
		{
			Material* material = command.material;

			if (material->type == Material::FORWARD && material->attribute.bShadowRecv)
			{
				// update shadow map textures lightwise
				for (unsigned int i = 0; i < lights.size(); ++i)
				{
					// TODO: cascaded shadow map
					// Set CSM properly (according to camera or so)
					material->RegisterTexture("lightShadowMap" + std::to_string(i + 1), lights[i]->shadow.GetFrameBuffer()->GetDepthStencilAttachment(0));
				}

				// find out relevant shaders
				shaders.insert(&material->shader);
			}
		}

		for (Shader* shader : shaders)
		{
			for (unsigned int i = 0; i < lights.size(); ++i)
			{
				if (lights[i]->useShadowCast)
				{
					shader->Bind();
					shader->SetUniform("UseParallelShadow", RenderConfig::UseParallelShadow());
					shader->SetUniform("lightShadowViewProjection" + std::to_string(i + 1), lights[i]->shadow.GetViewProj());
				}
			}
		}
	}

	void ForwardRenderer::RenderForwardCommands(const std::vector<RenderCommand>& commands)
	{
		for (const RenderCommand& command : commands)
		{
			Material* material = command.material;
			Mesh * mesh = command.mesh;

			material->shader.Bind();
			material->shader.SetUniform("model", command.transform);

			RenderMesh(mesh, material);
		}
	}

	void ForwardRenderer::RenderEmissionPointLights(const std::vector<PointLight*>& lights, Camera* camera, float radius)
	{
		m_volumnLightShader.Bind();

		for (PointLight* light : lights)
		{
			if (!light->useVolume) continue;

			float volumnRadius = radius < 0 ? light->radius : radius;

			if (!camera->IntersectFrustum(light->position, volumnRadius)) continue;

			glm::mat4 model;
			glm::mat4 translate = glm::translate(glm::mat4{}, light->position);
			glm::mat4 scale = glm::scale(glm::mat4{}, glm::vec3(volumnRadius));
			model = translate * scale;

			m_volumnLightShader.SetUniform("model", model);
			m_volumnLightShader.SetUniform("lightColor", glm::normalize(light->color) * light->intensity * 0.25f);

			RenderMesh(&m_sphere);
		}
	}

	void ForwardRenderer::RenderParticles(const std::vector<ParticleSystem*>& particles, Camera* camera)
	{
		for (ParticleSystem* ps : particles)
		{
			ps->UpdateTransform();

			if (!camera->IntersectFrustum(ps->aabbGlobal)) continue;

			ps->Render();
		}
	}
}