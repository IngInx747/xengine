#include "renderer.h"

#include <vendor/glad/glad.h>
#include <vendor/glm/glm.hpp>

#include <geometry/constant.h>

#include "ogl_status.h"
#include "texture_manager.h"
#include "render_config.h"
#include "general_renderer.h"
#include "forward_renderer.h"

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
		/// primary frame buffer

		// 1 color attachment
		m_mainCanvas.GenerateColorAttachments(1, 1, GL_HALF_FLOAT, 1);

		// 1 render buffer for depth test (in the case depth value is not needed by other routines)
		m_mainCanvas.GenerateDepthRenderBuffer(1, 1);

		// depth attachment (in the case depth value is needed)
		//m_canvas.GenerateDepthAttachment(1, 1, GL_HALF_FLOAT);

		/// secondary frame buffer

		// 1 color attachment
		m_swapCanvas.GenerateColorAttachments(1, 1, GL_HALF_FLOAT, 1);
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

			size = static_cast<unsigned int>(sizeof(glm::mat4) * 6 + sizeof(glm::vec4) * 4);
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
		blockCamera.CommitData(camera->GetProjection() * camera->GetPrevView());
		blockCamera.CommitData(camera->GetProjection());
		blockCamera.CommitData(camera->GetView());
		blockCamera.CommitData(glm::inverse(camera->GetProjection()));
		blockCamera.CommitData(glm::inverse(camera->GetView()));

		blockCamera.CommitData(camera->GetPosition());
		blockCamera.CommitData(camera->GetForward());
		blockCamera.CommitData(camera->GetUp());
		blockCamera.CommitData(camera->GetRight());

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
					RenderCommand command(&model->meshes[i], &model->materials[i]);
					command.transform = model->transform;
					command.prevTrans = model->prevTrans;
					command.aabb.BuildFromTransform(command.mesh->Aabb(), command.transform);

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

		m_mainCanvas.Resize(width, height);
		m_swapCanvas.Resize(width, height);

		deferredRenderer.Resize(width, height);
		ssaoRenderer.Resize(width, height);
		bloomRenderer.Resize(width, height);
		motionBlurRenderer.Resize(width, height);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Rendering methods (primary rendering pipeline)
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void Renderer::Render(Scene* scene, Camera* camera, FrameBuffer && target)
	{
		updateUniformBuffer(scene, camera);

		updateCommandBuffer(scene, camera);

		// default OpenGL settings
		OglStatus::SetBlend(GL_FALSE);
		OglStatus::SetCull(GL_TRUE);
		OglStatus::SetCullFace(GL_BACK);
		OglStatus::SetDepthTest(GL_TRUE);
		OglStatus::SetDepthFunc(GL_LESS);

		/// deferred pass
		{
			std::vector<RenderCommand> commands = commandManager.DeferredCommands(camera);

			OglStatus::SetPolygonMode(RenderConfig::UseWireframe() ? GL_LINE : GL_FILL);

			deferredRenderer.Generate(commands);

			OglStatus::SetPolygonMode(GL_FILL);
		}

		/// shadow maps
		if (RenderConfig::UseParallelShadow())
		{
			std::vector<RenderCommand> commands = commandManager.ShadowCastCommands();

			forwardRenderer.GenerateParallelShadow(commands, scene->parallelLights, camera);
		}

		/// per-lighting pass
		{
			OglStatus::SetBlend(GL_FALSE);

			if (RenderConfig::UseSSAO())
				ssaoRenderer.Generate(deferredRenderer.GetTexPosition(), deferredRenderer.GetTexNormal(), camera);

			if (RenderConfig::UseMotionBlur())
			{
				motionBlurRenderer.Generate(deferredRenderer.GetTexPosition(), camera);
				motionBlurRenderer.AttachMotion(deferredRenderer.GetTexMotion());
			}
		}

		/// deferred lighting
		{
			m_mainCanvas.Bind(); glViewport(0, 0, m_mainCanvas.Width(), m_mainCanvas.Height());
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			if (RenderConfig::UseSSR()) deferredRenderer.RenderReflectLight(m_swapCanvas.GetColorAttachment(0));

			deferredRenderer.RenderAmbientLight(scene->irradianceMap, scene->reflectionMap, ssaoRenderer.GetAO());

			deferredRenderer.RenderParallelLights(scene->parallelLights, camera, ssaoRenderer.GetAO());

			deferredRenderer.RenderPointLights(scene->pointLights, camera);
		}

		/// forward pass
		{
			Blit(deferredRenderer.GetFrameBuffer(), m_mainCanvas, GL_DEPTH_BUFFER_BIT); // copy depth buffer

			std::vector<RenderCommand> commands = commandManager.ForwardCommands(camera);

			ForwardRenderer::SetParallelShadow(scene->parallelLights, commands);

			m_mainCanvas.Bind(); glViewport(0, 0, m_mainCanvas.Width(), m_mainCanvas.Height());

			OglStatus::SetPolygonMode(RenderConfig::UseWireframe() ? GL_LINE : GL_FILL);

			ForwardRenderer::RenderForwardCommands(commands);

			OglStatus::SetPolygonMode(GL_FILL);
			OglStatus::SetCull(GL_TRUE);
			OglStatus::SetCullFace(GL_BACK);

			if (RenderConfig::UseRenderLights())
				forwardRenderer.RenderEmissionPointLights(scene->pointLights, camera, 0.25f);

			forwardRenderer.RenderParticles(scene->particles, camera);
		}

		/// alpha pass
		{
			std::vector<RenderCommand> commands = commandManager.AlphaCommands(camera);

			ForwardRenderer::SetParallelShadow(scene->parallelLights, commands);

			m_mainCanvas.Bind(); glViewport(0, 0, m_mainCanvas.Width(), m_mainCanvas.Height());

			OglStatus::SetPolygonMode(RenderConfig::UseWireframe() ? GL_LINE : GL_FILL);

			ForwardRenderer::RenderForwardCommands(commands);

			OglStatus::SetPolygonMode(GL_FILL);
		}

		/// post-lighting pass
		{
			OglStatus::SetBlend(GL_FALSE);

			if (RenderConfig::UseBloom())
				bloomRenderer.Generate(m_mainCanvas.GetColorAttachment(0));
		}

		/// visualization pass
		{
			if (RenderConfig::UseLightVolume())
			{
				m_mainCanvas.Bind(); glViewport(0, 0, m_mainCanvas.Width(), m_mainCanvas.Height());

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

		/// post-processing pass
		{
			// src: main canvas
			// dst: swap canvas

			if (RenderConfig::UseMotionBlur())
			{
				m_swapCanvas.Bind(); glViewport(0, 0, m_swapCanvas.Width(), m_swapCanvas.Height());
				motionBlurRenderer.Render(m_mainCanvas.GetColorAttachment(0));
				std::swap(m_mainCanvas, m_swapCanvas);
			}

			if (RenderConfig::UseBloom())
			{
				m_swapCanvas.Bind(); glViewport(0, 0, m_swapCanvas.Width(), m_swapCanvas.Height());
				bloomRenderer.Render(m_mainCanvas.GetColorAttachment(0));
				std::swap(m_mainCanvas, m_swapCanvas);
			}

			{
				m_swapCanvas.Bind(); glViewport(0, 0, m_swapCanvas.Width(), m_swapCanvas.Height());
				postRenderer.GenerateEffect(m_mainCanvas.GetColorAttachment(0));
				std::swap(m_mainCanvas, m_swapCanvas);
			}
		}

		/// end of frame
		{
			// synchronize (store result by the end of current frame)
			Blit(m_mainCanvas, m_swapCanvas, GL_COLOR_BUFFER_BIT);
			
			// restore front/back buffer pointers
			if (!m_mainCanvas.HasDepth())
				std::swap(m_mainCanvas, m_swapCanvas);

			// blit final result
			// Note: This step MUST be done at the END of rendering as the target frame buffer
			// is bound to DRAW after that.
			if (target)
				Blit(m_mainCanvas, target, GL_COLOR_BUFFER_BIT); // blit to assigned frame buffer
			else
				Blit(m_mainCanvas, width, height, GL_COLOR_BUFFER_BIT); // blit to default frame buffer
		}
	}
}