#include "cubic_capture.h"

#include <glad/glad.h>

#include <geometry/constant.h>

namespace xengine
{
	CubicCapture::CubicCapture()
	{
		SetCapturePosition({ 0.0f,  0.0f,  0.0f });
		setCaptureView();
	}

	CubicCapture::CubicCapture(const CubicCapture & other)
	{
		captures = other.captures;
		sideLen = other.sideLen;
	}

	CubicCapture & CubicCapture::operator=(const CubicCapture & other)
	{
		captures = other.captures;
		sideLen = other.sideLen;

		return *this;
	}

	CubicCapture::CubicCapture(const glm::vec3 & position)
	{
		SetCapturePosition(position);
		setCaptureView();
	}

	void CubicCapture::BindFace(unsigned int face, unsigned int mipmap)
	{
		captures.BindCubeMapFaceColorAttachment(0, face, 0, mipmap);
	}

	void CubicCapture::Unbind()
	{
		captures.Unbind();
	}

	void CubicCapture::Resize(unsigned int side_length)
	{
		sideLen = side_length;
		captures.Resize(side_length, side_length);
	}

	void CubicCapture::SetCapturePosition(const glm::vec3 & position)
	{
		cameras[0].SetCamera(position, {  1.0f,  0.0f,  0.0f }, { 0.0f, -1.0f,  0.0f });
		cameras[1].SetCamera(position, { -1.0f,  0.0f,  0.0f }, { 0.0f, -1.0f,  0.0f });
		cameras[2].SetCamera(position, {  0.0f,  1.0f,  0.0f }, { 0.0f,  0.0f,  1.0f });
		cameras[3].SetCamera(position, {  0.0f, -1.0f,  0.0f }, { 0.0f,  0.0f, -1.0f });
		cameras[4].SetCamera(position, {  0.0f,  0.0f,  1.0f }, { 0.0f, -1.0f,  0.0f });
		cameras[5].SetCamera(position, {  0.0f,  0.0f, -1.0f }, { 0.0f, -1.0f,  0.0f });
	}

	void CubicCapture::setCaptureView()
	{
		cameras[0].SetProjPerspective(kPi2, 1.0f, 0.1f, 100.0f);
		cameras[1].SetProjPerspective(kPi2, 1.0f, 0.1f, 100.0f);
		cameras[2].SetProjPerspective(kPi2, 1.0f, 0.1f, 100.0f);
		cameras[3].SetProjPerspective(kPi2, 1.0f, 0.1f, 100.0f);
		cameras[4].SetProjPerspective(kPi2, 1.0f, 0.1f, 100.0f);
		cameras[5].SetProjPerspective(kPi2, 1.0f, 0.1f, 100.0f);
	}

	void CubicCapture::GenerateCubeMap(unsigned int side_length)
	{
		sideLen = side_length;

		// use 6 color attachments to receive render result
		//captures.GenerateColorAttachments(128, 128, GL_FLOAT, 6);

		// use 1 color cube map to receive render result
		captures.GenerateCubeMapColorAttachments(sideLen, sideLen, GL_FLOAT, 1);

		// render buffer for depth test
		captures.GenerateDepthRenderBuffer(sideLen, sideLen);
	}
}