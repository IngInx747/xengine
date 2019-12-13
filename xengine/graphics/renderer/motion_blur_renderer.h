#pragma once
#ifndef XE_MOTION_BLUR_RENDERER_H
#define XE_MOTION_BLUR_RENDERER_H

#include <camera/camera.h>
#include <mesh/mesh.h>

#include "../shader/shader.h"
#include "../texture/texture.h"
#include "../frame_buffer/frame_buffer.h"

namespace xengine
{
	// Screen Space Motion Blur
	// Object blur by changing camera: Yes
	// Object blur by moving object: No
	// Background blur by moving camera: Yes

	class MotionBlurRenderer
	{
	public:
		MotionBlurRenderer();

		// resize frame buffer
		void Resize(unsigned int width, unsigned int height);

		// generate effect
		void Generate(Texture* source, Camera* camera);

		// cast effect onto target
		void Render(Texture* source);

		// attach another motion texture
		void AttachMotion(Texture* source);

	private:
		// result buffer(s)
		FrameBuffer m_target0;
		FrameBuffer m_target1;

		// relates shader(s)
		Shader m_captureShader; // capture camera motion
		Shader m_blitShader; // add another motion info
		Shader m_postShader; // cast motion blur effect

		//
		FrameBuffer* m_currBuffer;
		FrameBuffer* m_swapBuffer;

		// canvas
		Mesh* m_quad;
	};
}

#endif // !XE_MOTION_BLUR_RENDERER_H
