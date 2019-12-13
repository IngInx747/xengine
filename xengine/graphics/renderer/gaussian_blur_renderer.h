#ifndef XE_GAUSSIAN_BLUR_RENDERER_H
#define XE_GAUSSIAN_BLUR_RENDERER_H

#include <mesh/mesh.h>

#include "../shader/shader.h"
#include "../texture/texture.h"
#include "../frame_buffer/frame_buffer.h"

namespace xengine
{
	class GaussianBlurRenderer
	{
	public:
		GaussianBlurRenderer();

		// get blurring results
		inline Texture* GetSample_1_8() { return m_output0; }
		inline Texture* GetSample_1_16() { return m_output1; }

		// resize frame buffer
		void Resize(unsigned int width, unsigned int height);

		// generate the blurring results
		void Generate(Texture* source);

	private:
		// render target(s)
		FrameBuffer m_target0;
		FrameBuffer m_target1;

		// intermidiate target(s)
		FrameBuffer m_medium0;
		FrameBuffer m_medium1;
	
		// relates shader(s)
		Shader m_genShader;

		// render result(s)
		Texture* m_output0;
		Texture* m_output1;
	};
}

#endif // !XE_GAUSSIAN_BLUR_RENDERER_H
