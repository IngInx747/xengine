#pragma once
#ifndef XE_DEFERRED_RENDERER_H
#define XE_DEFERRED_RENDERER_H

#include <camera/camera.h>

#include "frame_buffer.h"
#include "render_command.h"
#include "light.h"

namespace xengine
{
	class DeferredRenderer
	{
	public:
		DeferredRenderer();

		// resize frame buffer
		void Resize(unsigned int width, unsigned int height);

		// render scene to get geometry information
		void Generate(const std::vector<RenderCommand>& commands);

		// render deferred parallel lights
		void RenderParallelLights(const std::vector<ParallelLight*>& lights, Camera* camera, Texture * ao);

		// render deferred volumn point lights
		void RenderPointLights(const std::vector<PointLight*>& lights, Camera* camera);

		// render deferred ambient light (Image-based lighting environment)
		void RenderAmbientLight(CubeMap* irradiance, CubeMap* reflection, Texture * ao);

		// render reflect light (Screen-space reflection)
		void RenderReflectLight(Texture* last_frame);

		inline FrameBuffer* GetFrameBuffer() { return &m_gBuffer; }
		inline Texture* GetTexPosition() { return m_gBuffer.GetColorAttachment(0); }
		inline Texture* GetTexNormal() { return m_gBuffer.GetColorAttachment(1); }
		inline Texture* GetTexAlbedo() { return m_gBuffer.GetColorAttachment(2); }
		inline Texture* GetTexPbrParam() { return m_gBuffer.GetColorAttachment(3); }
		inline Texture* GetTexMotion() { return m_gBuffer.GetColorAttachment(4); }

	public:

	private:
		// related frame buffer
		FrameBuffer m_gBuffer; // geometry buffer

		// related shaders
		Shader* m_parallelLightShader; // deferred parallel light shader
		Shader* m_pointLightShader; // deferred point light shader
		Shader* m_ambientLightShader; // deferred ambient light shader
		Shader* m_reflectLightShader; // deferred reflect light shader

		// related primitives
		Mesh* m_quad; // mesh for g-buffer quad sampling (parallel light)
		Mesh* m_sphere; // mesh for g-buffer spheric sampling (volumn point light)
	};
}

#endif // !XE_DEFERRED_RENDERER_H
