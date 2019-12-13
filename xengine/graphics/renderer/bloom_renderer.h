#ifndef XE_BLOOM_RENDERER_H
#define XE_BLOOM_RENDERER_H

#include <mesh/mesh.h>

#include "../shader/shader.h"
#include "../texture/texture.h"
#include "../frame_buffer/frame_buffer.h"

namespace xengine
{
	class BloomRenderer
	{
	public:
		BloomRenderer();

		// resize frame buffer
		void Resize(unsigned int width, unsigned int height);

		// generate effect
		void Generate(Texture* source);

		// cast effect onto target
		void Render(Texture* source);

		inline Texture* GetSample_1_2() { return m_output0; }
		inline Texture* GetSample_1_4() { return m_output1; }
		inline Texture* GetSample_1_8() { return m_output2; }
		inline Texture* GetSample_1_16() { return m_output3; }

	private:
		// result buffer(s)
		FrameBuffer m_target0;
		FrameBuffer m_target1;
		FrameBuffer m_target2;
		FrameBuffer m_target3;

		// intermidiate buffer(s)
		FrameBuffer m_medium0;
		FrameBuffer m_medium1;
		FrameBuffer m_medium2;
		FrameBuffer m_medium3;
		FrameBuffer m_origin;

		// relates shader(s)
		Shader m_filterShader;
		Shader m_blurShader;
		Shader m_postShader;

		// render result(s)
		Texture* m_output0;
		Texture* m_output1;
		Texture* m_output2;
		Texture* m_output3;

		// canvas
		Mesh* m_quad;
	};
}

#endif // !XE_FORWARD_RENDERER_H
