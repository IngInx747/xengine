#include "frame_buffer.h"

#include <cassert>

#include <vendor/glad/glad.h>

#include <utility/log.h>

namespace xengine
{
	////////////////////////////////////////////////////////////////
	// Frame Buffer Unique Instance
	////////////////////////////////////////////////////////////////

	FrameBufferMemory::FrameBufferMemory()
		:
		SharedMemory()
	{
	}

	FrameBufferMemory::~FrameBufferMemory()
	{
		Destory();
	}

	void FrameBufferMemory::Generate()
	{
		if (!fbo)
		{
			glGenFramebuffers(1, &fbo);
			Log::Message("[FrameBufferMemory] FrameBuffer " + std::to_string(fbo) + " generated", Log::DEBUG);
		}
	}

	void FrameBufferMemory::Destory()
	{
		if (fbo)
		{
			Log::Message("[FrameBufferMemory] FrameBuffer " + std::to_string(fbo) + " deleted", Log::DEBUG);

			if (rbo)
			{
				glDeleteRenderbuffers(1, &rbo);
				rbo = 0;
			}

			glDeleteFramebuffers(1, &fbo);
			fbo = 0;
		}
	}

	////////////////////////////////////////////////////////////////
	// Frame Buffer
	////////////////////////////////////////////////////////////////

	FrameBuffer::FrameBuffer()
		:
		SharedHandle()
	{
	}

	FrameBuffer::~FrameBuffer()
	{
	}

	FrameBuffer::FrameBuffer(const FrameBuffer & other)
		:
		SharedHandle(other)
	{
		m_ptr = other.m_ptr;
	}

	FrameBuffer & FrameBuffer::operator=(const FrameBuffer & other)
	{
		this->SharedHandle::operator=(other);
		m_ptr = other.m_ptr;
		return *this;
	}

	void FrameBuffer::allocateMemory()
	{
		if (m_ptr) return;

		m_ptr = new FrameBufferMemory;
		SharedHandle::Register(m_ptr);
	}

	void FrameBuffer::generateObject()
	{
		m_ptr->Generate();
	}

	void FrameBuffer::generate()
	{
		allocateMemory();
		generateObject();
	}

	Texture & FrameBuffer::GetColorAttachment(unsigned int i)
	{
		assert(i < m_ptr->colors.size());
		return m_ptr->colors[i];
	}

	const Texture & FrameBuffer::GetColorAttachment(unsigned int i) const
	{
		assert(i < m_ptr->colors.size());
		return m_ptr->colors[i];
	}

	Texture & FrameBuffer::GetDepthStencilAttachment(unsigned int i)
	{
		assert(i < m_ptr->depths.size());
		return m_ptr->depths[i];
	}

	const Texture & FrameBuffer::GetDepthStencilAttachment(unsigned int i) const
	{
		assert(i < m_ptr->depths.size());
		return m_ptr->depths[i];
	}

	void FrameBuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_ptr->fbo);
	}

	void FrameBuffer::BindColorAttachment(unsigned int attachment_id, unsigned int color_id, unsigned int mipmap)
	{
		if (m_ptr->colors.size() <= attachment_id || m_ptr->colors[attachment_id].Target() != GL_TEXTURE_2D) return;

		Bind();

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + color_id, GL_TEXTURE_2D, m_ptr->colors[attachment_id].ID(), mipmap);
	}

	void FrameBuffer::BindDepthAttachment(unsigned int attachment_id)
	{
		if (m_ptr->depths.size() <= attachment_id || m_ptr->depths[attachment_id].Target() != GL_TEXTURE_2D) return;

		Bind();

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_ptr->depths[attachment_id].ID(), 0);
	}

	void FrameBuffer::BindCubeMapFaceColorAttachment(unsigned int attachment_id, unsigned int face, unsigned int color_id, unsigned int mipmap)
	{
		if (m_ptr->colors.size() <= attachment_id || m_ptr->colors[attachment_id].Target() != GL_TEXTURE_CUBE_MAP) return;

		Bind();

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + color_id, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, m_ptr->colors[attachment_id].ID(), mipmap);
	}

	void FrameBuffer::BindCubeMapFaceDepthAttachment(unsigned int attachment_id, unsigned int face)
	{
		if (m_ptr->depths.size() <= attachment_id || m_ptr->depths[attachment_id].Target() != GL_TEXTURE_CUBE_MAP) return;

		Bind();

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, m_ptr->depths[attachment_id].ID(), 0);
	}

	void FrameBuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::Resize(unsigned int width, unsigned int height)
	{
		m_ptr->width = width;
		m_ptr->height = height;

		for (unsigned int i = 0; i < m_ptr->colors.size(); ++i)
		{
			m_ptr->colors[i].Resize(width, height);
		}

		for (unsigned int i = 0; i < m_ptr->depths.size(); ++i)
		{
			m_ptr->depths[i].Resize(width, height);
		}

		if (m_ptr->rbo > 0)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_ptr->fbo);
			glBindRenderbuffer(GL_RENDERBUFFER, m_ptr->rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_ptr->rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
		}
	}

	void FrameBuffer::GenerateColorAttachments(unsigned int width, unsigned int height, unsigned int data_type, unsigned int num_attachment)
	{
		generate();

		m_ptr->width = width;
		m_ptr->height = height;

		Bind();

		for (unsigned int i = 0; i < num_attachment; ++i)
		{
			Texture texture;

			GLenum internalFormat = GL_RGBA;
			if (data_type == GL_HALF_FLOAT)
				internalFormat = GL_RGBA16F;
			else if (data_type == GL_FLOAT)
				internalFormat = GL_RGBA32F;

			texture.SetFilterMin(GL_LINEAR);
			texture.SetFilterMax(GL_LINEAR);
			texture.SetWrapS(GL_CLAMP_TO_EDGE);
			texture.SetWrapT(GL_CLAMP_TO_EDGE);
			texture.SetMipmap(false);

			texture.Generate2D(width, height, internalFormat, GL_RGBA, data_type, 0);

			m_ptr->colors.push_back(texture);
		}

		// bind attachment [color_0] as default
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ptr->colors[0].ID(), 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			Log::Message("[FrameBuffer] Framebuffer (color attachments) not complete!", Log::ERROR);
		}

		Unbind();
	}

	void FrameBuffer::GenerateDepthAttachment(unsigned int width, unsigned int height, unsigned int data_type, unsigned int num_attachment)
	{
		generate();

		m_ptr->width = width;
		m_ptr->height = height;

		Bind();

		for (unsigned int i = 0; i < num_attachment; ++i)
		{
			Texture texture;

			texture.SetFilterMin(GL_NEAREST);
			texture.SetFilterMax(GL_NEAREST);
			texture.SetWrapS(GL_CLAMP_TO_EDGE);
			texture.SetWrapT(GL_CLAMP_TO_EDGE);
			texture.SetMipmap(false);

			texture.Generate2D(width, height, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, data_type, 0);

			m_ptr->depths.push_back(texture);
		}

		// bind attachment [depth_0] as default
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_ptr->depths[0].ID(), 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			Log::Message("[FrameBuffer] Framebuffer (depth) not complete!", Log::ERROR);
		}

		Unbind();
	}

	void FrameBuffer::GenerateDepthRenderBuffer(unsigned int width, unsigned height)
	{
		generate();

		m_ptr->width = width;
		m_ptr->height = height;

		if (m_ptr->rbo)
		{
			Log::Message("[FrameBuffer] RenderBuffer (depth) were occupied", Log::WARN);
			return;
		}

		Bind();

		glGenRenderbuffers(1, &m_ptr->rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, m_ptr->rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_ptr->rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			Log::Message("[FrameBuffer] Framebuffer (depth render buffer) not complete!", Log::ERROR);
		}

		Unbind();
	}

	void FrameBuffer::GenerateDepthStencilAttachment(unsigned int width, unsigned int height)
	{
		generate();

		m_ptr->width = width;
		m_ptr->height = height;

		Bind();

		Texture texture;

		texture.SetFilterMin(GL_NEAREST);
		texture.SetFilterMax(GL_NEAREST);
		texture.SetWrapS(GL_CLAMP_TO_EDGE);
		texture.SetWrapT(GL_CLAMP_TO_EDGE);
		texture.SetMipmap(false);

		texture.Generate2D(width, height, GL_DEPTH_STENCIL, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture.ID(), 0);

		m_ptr->depths.push_back(texture);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			Log::Message("[FrameBuffer] Framebuffer (depth-stencil) not complete!", Log::ERROR);
		}

		Unbind();
	}

	void FrameBuffer::GenerateDepthStencilRenderBuffer(unsigned int width, unsigned height)
	{
		generate();

		m_ptr->width = width;
		m_ptr->height = height;

		if (m_ptr->rbo)
		{
			Log::Message("[FrameBuffer] RenderBuffer (depth-stencil) were occupied", Log::WARN);
			return;
		}

		// There are only 2 depth/stencil formats, each providing 8 stencil bits:
		// GL_DEPTH24_STENCIL8 and GL_DEPTH32F_STENCIL8.

		Bind();

		glGenRenderbuffers(1, &m_ptr->rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, m_ptr->rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_ptr->rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			Log::Message("[FrameBuffer] Framebuffer (depth-stencil render buffer) not complete!", Log::ERROR);
		}

		Unbind();
	}

	void FrameBuffer::GenerateCubeMapColorAttachments(unsigned int width, unsigned int height, unsigned int data_type, unsigned int num_attachment)
	{
		generate();

		m_ptr->width = width;
		m_ptr->height = height;

		Bind();

		for (unsigned int i = 0; i < num_attachment; ++i)
		{
			Texture texture;

			texture.SetFilterMin(GL_LINEAR);
			texture.SetFilterMax(GL_LINEAR);
			texture.SetWrapS(GL_CLAMP_TO_EDGE);
			texture.SetWrapT(GL_CLAMP_TO_EDGE);
			texture.SetWrapR(GL_CLAMP_TO_EDGE);
			texture.SetMipmap(false);

			texture.GenerateCube(width, height, GL_RGB, data_type, false);

			m_ptr->colors.push_back(texture);
		}

		// bind attachment [color_0], face [+X] as default
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, m_ptr->colors[0].ID(), 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			Log::Message("[FrameBuffer] Framebuffer (cubic color attachments) not complete!", Log::ERROR);
		}

		Unbind();
	}

	void FrameBuffer::GenerateCubeMapDepthAttachment(unsigned int width, unsigned int height, unsigned int data_format, unsigned int num_attachment)
	{
		generate();

		m_ptr->width = width;
		m_ptr->height = height;

		Bind();

		for (unsigned int i = 0; i < num_attachment; ++i)
		{
			Texture texture;

			texture.SetFilterMin(GL_NEAREST);
			texture.SetFilterMax(GL_NEAREST);
			texture.SetWrapS(GL_CLAMP_TO_EDGE);
			texture.SetWrapT(GL_CLAMP_TO_EDGE);
			texture.SetWrapR(GL_CLAMP_TO_EDGE);
			texture.SetMipmap(false);

			texture.GenerateCube(width, height, GL_DEPTH_COMPONENT, data_format, false);

			// require geometry shader support
			//glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture->ID(), 0);

			m_ptr->depths.push_back(texture);
		}

		// bind attachment [depth_0], face [+X] as default
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X, m_ptr->depths[0].ID(), 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			Log::Message("[FrameBuffer] Framebuffer (cubic depth) not complete!", Log::ERROR);
		}

		Unbind();
	}
}