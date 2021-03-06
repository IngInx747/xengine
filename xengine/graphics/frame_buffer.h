#pragma once
#ifndef XE_FRAME_BUFFER_H
#define XE_FRAME_BUFFER_H

#include <vector>
#include <memory>
#include <unordered_map>

#include <utility/smart_handle.h>

#include "texture.h"

namespace xengine
{
	class FrameBufferMemory : public SharedMemory
	{
	public:
		virtual void Generate(); // allocate memory on GPU
		virtual void Destory(); // free memory on GPU

	public:
		unsigned int width = 0;
		unsigned int height = 0;
		unsigned int fbo = 0; // frame buffer object
		unsigned int rbo = 0; // render buffer object (optional)
		std::vector<Texture> colors; // color attachments
		std::vector<Texture> depths; // depth / depth-stencil attachments
	};

	class FrameBuffer : public SharedHandle<FrameBufferMemory>
	{
	public:
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
		void Unbind();

		// resize buffer, re-allocate memory for attachments
		void Resize(unsigned int width, unsigned int height);

		// get i-th color attachment, null if not available
		Texture & GetColorAttachment(unsigned int i);
		const Texture & GetColorAttachment(unsigned int i) const;

		// get depth / depth-stencil attachment, null if not available
		Texture & GetDepthStencilAttachment(unsigned int i);
		const Texture & GetDepthStencilAttachment(unsigned int i) const;

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

		// Access

		// get number of color attachments
		inline unsigned int NumColor() const { return static_cast<unsigned int>(m_ptr->colors.size()); }

		// get number of depth attachments
		inline unsigned int NumDepth() const { return static_cast<unsigned int>(m_ptr->depths.size()); }

		// tell if depth attachment has been attached
		inline bool HasDepth() const { return m_ptr->rbo || m_ptr->depths.size(); }

		explicit operator bool() const { return m_ptr && m_ptr->fbo; }

		inline unsigned int ID() const { return m_ptr->fbo; }
		inline unsigned int Width() const { return m_ptr->width; }
		inline unsigned int Height() const { return m_ptr->height; }
	};
}

#endif // !XE_FRAME_BUFFER_H
