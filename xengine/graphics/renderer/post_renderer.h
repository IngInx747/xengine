#pragma once
#ifndef XE_POST_RENDERER_H
#define XE_POST_RENDERER_H

#include <mesh/mesh.h>

#include "../shader/shader.h"
#include "../texture/texture.h"
#include "../frame_buffer/frame_buffer.h"

namespace xengine
{
	class PostRenderer
	{
	public:
		PostRenderer();

		inline FrameBuffer* GetFrameBuffer() { return &m_target; }

		// resize frame buffer
		void Resize(unsigned int width, unsigned int height);

		// integrate both pre and post lighting effects
		void GenerateEffect(Texture* source, Texture* motion, Texture* bloom1, Texture* bloom2, Texture* bloom3, Texture* bloom4);

	private:
		// render target(s)
		FrameBuffer m_target;

		// relates shader(s)
		Shader* m_shader;

		// related mesh(es)
		Mesh* m_quad;
	};
}

#endif // !XE_POST_RENDERER_H
