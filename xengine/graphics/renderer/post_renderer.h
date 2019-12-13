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

		// integrate both pre and post lighting effects
		void GenerateEffect(Texture* source);

	private:
		// relates shader(s)
		Shader* m_shader;

		// related mesh(es)
		Mesh* m_quad;
	};
}

#endif // !XE_POST_RENDERER_H
