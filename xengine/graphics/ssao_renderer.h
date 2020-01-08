#ifndef XE_SSAO_RENDERER_H
#define XE_SSAO_RENDERER_H

#include <camera/camera.h>
#include <mesh/mesh.h>

#include "shader.h"
#include "texture.h"
#include "frame_buffer.h"

namespace xengine
{
	class SSAORenderer
	{
	public:
		SSAORenderer();

		// get ambient occlusion result
		inline const Texture & GetAO() { return m_output; }

		// resize frame buffer
		void Resize(unsigned int width, unsigned int height);

		// generate the ambient occlusion layout
		void Generate(const Texture & gPosition, const Texture & gNormal, Camera* camera);

	private:
		// render target(s)
		FrameBuffer m_target;

		// relates shader(s)
		Shader m_shader;

		// render result(s)
		Texture m_output;

		// related mesh(es)
		Mesh m_quad;

		// SSAO configure
		Texture m_noise; // noise texture
		int m_kernelSize; // sample count
	};
}

#endif // !XE_SSAO_RENDERER_H
