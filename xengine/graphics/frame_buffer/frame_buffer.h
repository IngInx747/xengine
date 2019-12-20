#pragma once
#ifndef XE_FRAME_BUFFER_H
#define XE_FRAME_BUFFER_H

#include <vector>
#include <memory>
#include <unordered_map>

#include <utility/smart_handle.h>

#include "../texture/texture.h"

namespace xengine
{
	class FrameBufferMemory : public SharedMemory
	{
	public:
		FrameBufferMemory();
		virtual ~FrameBufferMemory();

		// Note: As the struct holds unique resource, instance copy is not allowed
		FrameBufferMemory(const FrameBufferMemory& other) = delete;
		FrameBufferMemory & operator=(const FrameBufferMemory& other) = delete;

		void Generate(); // allocate memory on GPU
		void Destory(); // free memory on GPU

	public:
		unsigned int width = 0;
		unsigned int height = 0;
		unsigned int fbo = 0; // frame buffer object
		unsigned int rbo = 0; // render buffer object (optional)
		std::vector<Texture> colors; // color attachments
		std::vector<Texture> depths; // depth / depth-stencil attachments
	};

	class FrameBuffer : public SharedHandle
	{
	public:
		FrameBuffer();
		virtual ~FrameBuffer();

		FrameBuffer(const FrameBuffer& other);
		FrameBuffer & operator=(const FrameBuffer& other);

		// bind the fbo
		void Bind();

		// bind the fbo and bind ith color attachment to GL_COLOR_ATTACHMENT[I]
		void BindColorAttachment(unsigned int attachment_id, unsigned int color_enum, unsigned int mipmap = 0);

		// bind the fbo and bind ith depth attachment to GL_DEPTH_COMPONENT
		void BindDepthAttachment(unsigned int attachment_id);

		// bind the fbo, bind ith color attachment to GL_COLOR_ATTACHMENT[I], set face for current render target
		void BindCubeMapFaceColorAttachment(unsigned int attachment_id, unsigned int face, unsigned int color_enum, unsigned int mipmap = 0);

		// bind the fbo, bind ith color attachment to GL_COLOR_ATTACHMENT[I], set face for current render target
		void BindCubeMapFaceDepthAttachment(unsigned int attachment_id, unsigned int face);

		// unbind / bind to default frame buffer
		static void Unbind();

		// resize buffer, re-allocate memory for attachments
		void Resize(unsigned int width, unsigned int height);

		// get i-th color attachment, null if not available
		Texture* GetColorAttachment(unsigned int i);

		// get depth / depth-stencil attachment, null if not available
		Texture* GetDepthStencilAttachment(unsigned int i);

		// 2D

		// generate color attachment(s) and attach to the frame buffer
		void GenerateColorAttachments(unsigned int width, unsigned int height, unsigned int data_type, unsigned int num_attachment = 1);

		// generate depth attachment (for shadow map) and attach to the frame buffer
		void GenerateDepthAttachment(unsigned int width, unsigned int height, unsigned int data_type, unsigned int num_attachment = 1);

		// generate depth-stencil render buffer and attach to the frame buffer
		void GenerateDepthRenderBuffer(unsigned int width, unsigned height);

		// generate depth-stencil attachment and attach to the frame buffer
		void GenerateDepthStencilAttachment(unsigned int width, unsigned int height);

		// generate depth-stencil render buffer and attach to the frame buffer
		void GenerateDepthStencilRenderBuffer(unsigned int width, unsigned height);

		// Cubic

		// generate cube map color attachment(s) and attach to the frame buffer
		void GenerateCubeMapColorAttachments(unsigned int width, unsigned int height, unsigned int data_type, unsigned int num_attachment = 1);

		// generate cube map depth attachment alone (for point light shadow map)
		void GenerateCubeMapDepthAttachment(unsigned int width, unsigned int height, unsigned int data_type, unsigned int num_attachment = 1);

		inline unsigned int ID() const { return m_ptr->fbo; }
		inline unsigned int Width() const { return m_ptr->width; }
		inline unsigned int Height() const { return m_ptr->height; }

	private:
		void generate();

	private:
		FrameBufferMemory* m_ptr = nullptr;
	};
}

#endif // !XE_FRAME_BUFFER_H
