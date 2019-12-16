#pragma once
#ifndef XE_CUBIC_CAPTURE_H
#define XE_CUBIC_CAPTURE_H

#include <vendor/glm/common.hpp>

#include <camera/camera.h>

#include "../frame_buffer/frame_buffer.h"

namespace xengine
{
	class CubicCapture
	{
	public:
		CubicCapture();
		CubicCapture(const CubicCapture& other);
		CubicCapture & operator=(const CubicCapture& other);

		CubicCapture(const glm::vec3& position);

		// bind one face of the cube map to be active render target
		void BindFace(unsigned int face, unsigned int mipmap = 0);

		// unbind cubemap
		void Unbind();

		// resize capture side length
		void Resize(unsigned int side_length);

		// set the position where capture happens
		void SetCapturePosition(const glm::vec3& position);

		// generate cubemap texture to restore capture
		void GenerateCubeMap(unsigned int side_length);

		unsigned int Width() { return sideLen; }
		unsigned int Height() { return sideLen; }

	private:
		void setCaptureView();

	public:
		FrameBuffer captures;

		// 6-facing camera
		Camera cameras[6];

		// side length of capture frame buffer
		unsigned int sideLen;
	};
}

#endif // !XE_CUBIC_CAPTURE_H
