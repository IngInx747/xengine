#include "renderer.h"

#include <vendor/glad/glad.h>
#include <vendor/glm/glm.hpp>

#include <geometry/constant.h>

#include "../platform/ogl_status.h"
#include "render_config.h"
#include "general_renderer.h"
#include "forward_renderer.h"

#include "../texture/texture_manager.h"

namespace xengine
{
	UniformBuffer Renderer::ubCamera;
	UniformBuffer Renderer::ubLights;
	UniformBlock Renderer::blockCamera;
	UniformBlock Renderer::blockParallelLights;
	UniformBlock Renderer::blockPointLights;

	void Renderer::Initialize()
	{
		// uniform block
		generateUniformBuffer();

		// TO BE DECIDED...
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// On-render methods (execute before main loop)
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Renderer::Renderer()
	{
		// primary frame buffer

		// 1 color attachment
		m_canvas.GenerateColorAttachments(1, 1, GL_HALF_FLOAT, 1);

		// 1 render buffer for depth test (in the case depth value is not needed by other routines)
		m_canvas.GenerateDepthRenderBuffer(1, 1);

		// depth attachment (in the case depth value is needed)
		//m_canvas.GenerateDepthAttachment(1, 1, GL_HALF_FLOAT);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// On-render methods (execute within main loop)
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void Renderer::generateUniformBuffer()
	{
		// block should be set according to "uniform.glsl"
		// camera
		{
			unsigned int size;
			unsigned int offset = 0;

			size = static_cast<unsigned int>(sizeof(glm::mat4) * 5 + sizeof(glm::vec4));
			blockCamera.Register(&ubCamera);
			blockCamera.SetBlock(offset, size);
			offset += size;

			ubCamera.Generate(offset, 0);
		}

		// lights
		{
			unsigned int size;
			unsigned int offset = 0;

			// parallel lights
			size = static_cast<unsigned int>(4 * (sizeof(glm::vec4) * 2));
			blockParallelLights.Register(&ubLights);
			blockParallelLights.SetBlock(offset, size);
			offset += size;

			// point lights
			size = static_cast<unsigned int>(8 * (sizeof(glm::vec4) * 2));
			blockPointLights.Register(&ubLights);
			blockPointLights.SetBlock(offset, size);
			offset += size;

			ubLights.Generate(offset, 1);
		}
	}

	void Renderer::updateUniformBuffer(Scene* scene, Camera* camera)
	{
		// camera
		blockCamera.Refresh();
		blockCamera.CommitData(camera->GetProjection() * camera->GetView());
		blockCamera.CommitData(camera->GetProjection() * camera->GetPreviousView());
		blockCamera.CommitData(camera->GetProjection());
		blockCamera.CommitData(camera->GetView());
		blockCamera.CommitData(glm::inverse(camera->GetView()));
		blockCamera.CommitData(camera->GetPosition());

		// parallel lights
		blockParallelLights.Refresh();

		for (unsigned int i = 0; i < scene->parallelLights.size() && i < 4; ++i)
		{
			ParallelLight* light = scene->parallelLights[i];
			blockParallelLights.CommitData(light->direction);
			blockParallelLights.CommitData(light->color);
		}

		// point lights
		blockPointLights.Refresh();

		for (unsigned int i = 0; i < scene->pointLights.size() && i < 8; ++i)
		{
			PointLight* light = scene->pointLights[i];
			blockPointLights.CommitData(light->position);
			blockPointLights.CommitData(light->color);
		}
	}

	void Renderer::generateCommandsFromScene(Scene* scene)
	{
		for (Model* root : scene->models)
		{
			std::vector<Model*> recStack;
			recStack.push_back(root);

			while (!recStack.empty())
			{
				Model* model = recStack.back();
				recStack.pop_back();

				// make sure model geometry is up-to-date
				model->UpdateTransform();

				// generate a render command and push to queue
				for (size_t i = 0; i < model->meshes.size(); ++i)
				{
					RenderCommand command;
					command.mesh = model->meshes[i];
					command.material = model->materials[i];
					command.transform = model->transform.transform;
					command.prevTransform = model->transform.prevTransform;
					command.aabb.BuildFromTransform(command.mesh->aabb, command.transform);

					commandManager.Push(command);
				}

				// continue to render all children of current model
				for (auto child : model->children)
					recStack.push_back(child);
			}
		}
	}

	void Renderer::updateCommandBuffer(Scene* scene, Camera* camera)
	{
		// TODO:
		// not necessary to update each frame
		// send signal to update only when scene is changed
		// Make command.transform pointer to model.transform
		commandManager.Clear();

		// generate render commands from scene
		generateCommandsFromScene(scene);

		// sort commands against shader id to save OpenGL state switch
		commandManager.SortOnShaderIndex(); // not necessary
	}

	void Renderer::Resize(unsigned width, unsigned int height)
	{
		this->width = width;
		this->height = height;

		m_canvas.Resize(width, height);

		deferredRenderer.Resize(width, height);
		ssaoRenderer.Resize(width, height);
		downsampleRenderer.Resize(width, height);
		gaussianBlurRenderer.Resize(width, height);
		bloomRenderer.Resize(width, height);
		postRenderer.Resize(width, height);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Rendering methods (primary rendering pipeline)
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void Renderer::Render(Scene* scene, Camera* camera, FrameBuffer* target)
	{
		FrameBuffer* fBuffer = &m_canvas;

		updateUniformBuffer(scene, camera);

		updateCommandBuffer(scene, camera);

		// default OpenGL settings
		OglStatus::SetBlend(GL_FALSE);
		OglStatus::SetCull(GL_TRUE);
		OglStatus::SetCullFace(GL_BACK);
		OglStatus::SetDepthTest(GL_TRUE);
		OglStatus::SetDepthFunc(GL_LESS);

		// 1. deferred pass
		{
			std::vector<RenderCommand> commands = commandManager.DeferredCommands(camera);

			OglStatus::SetPolygonMode(RenderConfig::UseWireframe() ? GL_LINE : GL_FILL);

			deferredRenderer.GenerateGeometry(commands);

			OglStatus::SetPolygonMode(GL_FILL);
		}

		// 2. shadow maps
		if (RenderConfig::UseShadow())
		{
			std::vector<RenderCommand> commands = commandManager.ShadowCastCommands();

			forwardRenderer.GenerateShadowParallelLights(commands, scene->parallelLights, camera);
		}

		// 3. per-lighting pass
		if (RenderConfig::UseSSAO())
		{
			OglStatus::SetBlend(GL_FALSE);

			ssaoRenderer.Generate(deferredRenderer.GetTexPosition(), deferredRenderer.GetTexNormal(), camera);
		}

		// 4. deferred lighting
		{
			fBuffer->Bind(); glViewport(0, 0, fBuffer->Width(), fBuffer->Height());
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			if (scene->irradianceMap && scene->reflectionMap) deferredRenderer.RenderAmbientLight(
				scene->irradianceMap, scene->reflectionMap, ssaoRenderer.GetAO());

			deferredRenderer.RenderParallelLights(scene->parallelLights, camera);

			deferredRenderer.RenderPointLights(scene->pointLights, camera);
		}

		// 5. forward pass
		{
			GeneralRenderer::Blit(deferredRenderer.GetFrameBuffer(), fBuffer, GL_DEPTH_BUFFER_BIT); // copy depth buffer

			std::vector<RenderCommand> commands = commandManager.ForwardCommands(camera);

			ForwardRenderer::SetShadowParallelLights(scene->parallelLights, commands);

			fBuffer->Bind(); glViewport(0, 0, fBuffer->Width(), fBuffer->Height());

			OglStatus::SetPolygonMode(RenderConfig::UseWireframe() ? GL_LINE : GL_FILL);

			ForwardRenderer::RenderForwardCommands(commands);

			OglStatus::SetPolygonMode(GL_FILL);
			OglStatus::SetCull(GL_TRUE);
			OglStatus::SetCullFace(GL_BACK);

			if (RenderConfig::UseRenderLights())
				forwardRenderer.RenderEmissionPointLights(scene->pointLights, camera, 0.25f);
		}

		// 6. alpha pass
		{
			std::vector<RenderCommand> commands = commandManager.AlphaCommands(camera);

			ForwardRenderer::SetShadowParallelLights(scene->parallelLights, commands);

			fBuffer->Bind(); glViewport(0, 0, fBuffer->Width(), fBuffer->Height());

			OglStatus::SetPolygonMode(RenderConfig::UseWireframe() ? GL_LINE : GL_FILL);

			ForwardRenderer::RenderForwardCommands(commands);

			OglStatus::SetPolygonMode(GL_FILL);
		}

		// 7. post-lighting pass
		{
			OglStatus::SetBlend(GL_FALSE);

			downsampleRenderer.Generate(fBuffer->GetColorAttachment(0));

			gaussianBlurRenderer.Generate(downsampleRenderer.GetSample_1_2());

			if (RenderConfig::UseBloom())
				bloomRenderer.Generate(fBuffer->GetColorAttachment(0));
		}

		// 8. visualization pass
		{
			if (RenderConfig::UseLightVolume())
			{
				fBuffer->Bind(); glViewport(0, 0, fBuffer->Width(), fBuffer->Height());

				OglStatus::SetPolygonMode(GL_LINE);
				OglStatus::SetCull(GL_TRUE);
				OglStatus::SetCullFace(GL_FRONT);

				forwardRenderer.RenderEmissionPointLights(scene->pointLights, camera);

				OglStatus::SetPolygonMode(GL_FILL);
				OglStatus::SetCullFace(GL_BACK);
			}

			if (RenderConfig::UseRenderProbes())
			{
				// TODO
			}
		}

		// 9. post-processing pass
		{
			postRenderer.GenerateEffect(
				fBuffer->GetColorAttachment(0),
				deferredRenderer.GetTexMotion(),
				bloomRenderer.GetSample_1_2(),
				bloomRenderer.GetSample_1_4(), 
				bloomRenderer.GetSample_1_8(), 
				bloomRenderer.GetSample_1_16());
		}

		// 10. blit to default frame buffer
		{
			if (target)
				GeneralRenderer::Blit(postRenderer.GetFrameBuffer(), target, GL_COLOR_BUFFER_BIT);
			else
				GeneralRenderer::Blit(postRenderer.GetFrameBuffer(), width, height, GL_COLOR_BUFFER_BIT);
		}
	}
}