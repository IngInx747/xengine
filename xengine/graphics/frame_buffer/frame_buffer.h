#pragma once
#ifndef XE_FRAME_BUFFER_H
#define XE_FRAME_BUFFER_H

#include <vector>
#include <memory>

#include "../texture/texture.h"

namespace xengine
{
	class FrameBuffer
	{
	public:
		struct Attribute
		{
			unsigned int width;
			unsigned int height;

			Attribute();
		};

	public:
		FrameBuffer();
		~FrameBuffer();

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

		// release memory
		void Destory();

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

		inline unsigned int ID() const { return m_fbo; }
		inline unsigned int Width() const { return attribute.width; }
		inline unsigned int Height() const { return attribute.height; }

	private:
		// generate frame buffer object, if not created before
		void generateFrameBuffer();

	public:
		Attribute attribute;

	private:
		// fbo (frame buffer object)
		unsigned int m_fbo = 0;

		// rbo (render buffer object)
		unsigned int m_rbo = 0; // optional

		// texture attachments
		std::vector<std::shared_ptr<Texture>> m_attachments;

		// color attachments
		std::vector<Texture*> m_colors;

		// depth / depth-stencil attachments
		std::vector<Texture*> m_depths;
	};
}

#endif // !XE_FRAME_BUFFER_H
