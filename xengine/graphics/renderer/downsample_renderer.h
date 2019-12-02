#ifndef XE_DOWNSAMPLE_RENDERER_H
#define XE_DOWNSAMPLE_RENDERER_H

#include <camera/camera.h>
#include <mesh/mesh.h>

#include "../shader/shader.h"
#include "../texture/texture.h"
#include "../frame_buffer/frame_buffer.h"

namespace xengine
{
	class DownsampleRenderer
	{
	public:
		DownsampleRenderer();

		// get downsampling results
		inline Texture* GetSample_1_2() { return m_output0; }
		inline Texture* GetSample_1_4() { return m_output1; }
		inline Texture* GetSample_1_8() { return m_output2; }
		inline Texture* GetSample_1_16() { return m_output3; }

		// resize frame buffer
		void Resize(unsigned int width, unsigned int height);

		// generate the downsampling results
		void Generate(Texture* source);

	private:
		// render target(s)
		FrameBuffer m_target0;
		FrameBuffer m_target1;
		FrameBuffer m_target2;
		FrameBuffer m_target3;

		// render result(s)
		Texture* m_output0;
		Texture* m_output1;
		Texture* m_output2;
		Texture* m_output3;

		// relates shader(s)
		Shader* m_shader;

		// related mesh(es)
		Mesh* m_quad;
	};
}

#endif // !XE_DOWNSAMPLE_RENDERER_H
