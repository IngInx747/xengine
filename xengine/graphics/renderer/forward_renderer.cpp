#include "forward_renderer.h"

#include <vendor/glad/glad.h>
#include <vendor/glm/glm.hpp>
#include <vendor/glm/gtc/matrix_transform.hpp>

#include <camera/camera.h>
#include <mesh/mesh.h>
#include <model/model.h>
#include <mesh/mesh_manager.h>

#include "../shader/shader_manager.h"
#include "../platform/ogl_status.h"
#include "render_config.h"
#include "general_renderer.h"

namespace xengine
{
	ForwardRenderer::ForwardRenderer()
	{
		m_parallelShadowShader = ShaderManager::LoadVertFragShader("shadow directional", "shaders/shadow_cast.vs", "shaders/shadow_cast.fs");
		m_volumnLightShader = ShaderManager::LoadVertFragShader("volumn light", "shaders/light.vs", "shaders/light.fs");
		m_sphere = MeshManager::LoadPrimitive("sphere", 16, 8);
	}

	void ForwardRenderer::GenerateShadowParallelLights(const std::vector<RenderCommand>& commands, const std::vector<ParallelLight*>& lights, Camera* camera)
	{
		Shader* shader = m_parallelShadowShader;

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

			shader->Use();
			shader->SetUniform("projection", shadow.GetProj());
			shader->SetUniform("view", shadow.GetView());

			// we only care about depth info so we don't use RenderCommand(...) which is more expensive
			for (const RenderCommand& command : commands)
			{
				if (!light->shadow.GetCamera()->IntersectFrustum(command.aabb)) continue;

				shader->SetUniform("model", command.transform);
				GeneralRenderer::RenderMesh(command.mesh);
			}
		}

		OglStatus::SetCullFace(GL_BACK); // restore original culling setting

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void ForwardRenderer::SetShadowParallelLights(const std::vector<ParallelLight*>& lights, const std::vector<RenderCommand>& commands)
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
				shaders.insert(material->shader);
			}
		}

		for (Shader* shader : shaders)
		{
			for (unsigned int i = 0; i < lights.size(); ++i)
			{
				if (lights[i]->useShadowCast)
				{
					shader->Use();
					shader->SetUniform("ShadowsEnabled", RenderConfig::UseShadow());
					shader->SetUniform("lightShadowViewProjection" + std::to_string(i + 1), lights[i]->shadow.GetViewProj());
				}
			}
		}
	}

	void ForwardRenderer::RenderForwardCommands(const std::vector<RenderCommand>& commands)
	{
		for (const RenderCommand& command : commands)
			GeneralRenderer::RenderSingleCommand(command);
	}

	void ForwardRenderer::RenderEmissionPointLights(const std::vector<PointLight*>& lights, Camera* camera, float radius)
	{
		Shader* shader = m_volumnLightShader;
		Mesh* sphere = m_sphere;

		shader->Use();

		for (PointLight* light : lights)
		{
			if (!light->useVolume) continue;

			float volumnRadius = radius < 0 ? light->radius : radius;

			if (!camera->IntersectFrustum(light->position, volumnRadius)) continue;

			glm::mat4 model;
			glm::mat4 translate = glm::translate(glm::mat4{}, light->position);
			glm::mat4 scale = glm::scale(glm::mat4{}, glm::vec3(volumnRadius));
			model = translate * scale;

			shader->SetUniform("model", model);
			shader->SetUniform("lightColor", glm::normalize(light->color) * light->intensity * 0.25f);

			GeneralRenderer::RenderMesh(sphere);
		}
	}
}