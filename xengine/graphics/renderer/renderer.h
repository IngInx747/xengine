#ifndef XE_RENDERER_H
#define XE_RENDERER_H

#include <memory>

#include <camera/camera.h>
#include <scene/scene.h>

#include "../command/render_command.h"
#include "../command/render_command_manager.h"
#include "../uniform_buffer/uniform_buffer.h"
#include "../uniform_buffer/uniform_block.h"
#include "../capture/cubic_capture.h"

#include "deferred_renderer.h"
#include "forward_renderer.h"
#include "bloom_renderer.h"
#include "motion_blur_renderer.h"
#include "ssao_renderer.h"
#include "post_renderer.h"

namespace xengine
{
	class Renderer
	{
	public:
		Renderer();

		// resize canvas and frame buffers
		void Resize(unsigned width, unsigned int height);

		// render scene to target frame buffer (default frame if target not given)
		void Render(Scene* scene, Camera* camera, FrameBuffer* target);

	public:
		static void Initialize();

	private:
		// generate render commands from scene
		void generateCommandsFromScene(Scene* scene);

		// update commands
		void updateCommandBuffer(Scene* scene, Camera* camera);

	private:
		// register uniform buffers
		static void generateUniformBuffer();

		// update uniform buffers (for all renderers)
		static void updateUniformBuffer(Scene* scene, Camera* camera);

	public:// canvas
		unsigned int width;
		unsigned int height;

	private:
		// commands
		RenderCommandManager commandManager;

		// deferred renderer
		DeferredRenderer deferredRenderer;

		// forward renderer
		ForwardRenderer forwardRenderer;

		// screen space renderers
		SSAORenderer ssaoRenderer;
		BloomRenderer bloomRenderer;
		MotionBlurRenderer motionBlurRenderer;
		
		// post renderer
		PostRenderer postRenderer;

		// related frame buffer(s)
		FrameBuffer m_framebuffer0; // primary frame buffer
		FrameBuffer m_framebuffer1; // secondary frame buffer
		FrameBuffer* m_mainCanvas;
		FrameBuffer* m_swapCanvas;

	private:
		// uniform buffer objects
		static UniformBuffer ubCamera;
		static UniformBuffer ubLights;

		// uniform buffer agents
		static UniformBlock blockCamera;
		static UniformBlock blockParallelLights;
		static UniformBlock blockPointLights;
	};
}

#endif // !XE_RENDERER_H
