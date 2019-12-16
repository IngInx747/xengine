#include "frame_buffer.h"

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

			attachments.clear();

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
		m_ptr = new FrameBufferMemory;

		SharedHandle::Register(m_ptr);
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

	FrameBuffer::~FrameBuffer()
	{
	}

	Texture* FrameBuffer::GetColorAttachment(unsigned int i)
	{
		if (i >= m_ptr->colors.size()) return nullptr;
		return m_ptr->colors[i];
	}

	Texture* FrameBuffer::GetDepthStencilAttachment(unsigned int i)
	{
		if (i >= m_ptr->depths.size()) return nullptr;
		return m_ptr->depths[i];
	}

	void FrameBuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_ptr->fbo);
	}

	void FrameBuffer::BindColorAttachment(unsigned int attachment_id, unsigned int color_id, unsigned int mipmap)
	{
		if (m_ptr->colors.size() <= attachment_id || m_ptr->colors[attachment_id]->attribute.target != GL_TEXTURE_2D) return;

		Bind();

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + color_id, GL_TEXTURE_2D, m_ptr->colors[attachment_id]->ID(), mipmap);
	}

	void FrameBuffer::BindDepthAttachment(unsigned int attachment_id)
	{
		if (m_ptr->depths.size() <= attachment_id || m_ptr->depths[attachment_id]->attribute.target != GL_TEXTURE_2D) return;

		Bind();

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_ptr->depths[attachment_id]->ID(), 0);
	}

	void FrameBuffer::BindCubeMapFaceColorAttachment(unsigned int attachment_id, unsigned int face, unsigned int color_id, unsigned int mipmap)
	{
		if (m_ptr->colors.size() <= attachment_id || m_ptr->colors[attachment_id]->attribute.target != GL_TEXTURE_CUBE_MAP) return;

		Bind();

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + color_id, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, m_ptr->colors[attachment_id]->ID(), mipmap);
	}

	void FrameBuffer::BindCubeMapFaceDepthAttachment(unsigned int attachment_id, unsigned int face)
	{
		if (m_ptr->depths.size() <= attachment_id || m_ptr->depths[attachment_id]->attribute.target != GL_TEXTURE_CUBE_MAP) return;

		Bind();

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, m_ptr->depths[attachment_id]->ID(), 0);
	}

	void FrameBuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::Resize(unsigned int width, unsigned int height)
	{
		m_ptr->width = width;
		m_ptr->height = height;

		for (unsigned int i = 0; i < m_ptr->attachments.size(); ++i)
		{
			m_ptr->attachments[i]->Resize(width, height);
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
		m_ptr->width = width;
		m_ptr->height = height;
		m_ptr->Generate();

		Bind();

		for (unsigned int i = 0; i < num_attachment; ++i)
		{
			std::shared_ptr<Texture> texture = std::make_shared<Texture>();

			texture->attribute.filterMin = GL_LINEAR;
			texture->attribute.filterMax = GL_LINEAR;
			texture->attribute.wrapS = GL_CLAMP_TO_EDGE;
			texture->attribute.wrapT = GL_CLAMP_TO_EDGE;
			texture->attribute.mipmapping = false;

			GLenum internalFormat = GL_RGBA;
			if (data_type == GL_HALF_FLOAT)
				internalFormat = GL_RGBA16F;
			else if (data_type == GL_FLOAT)
				internalFormat = GL_RGBA32F;

			texture->Generate2D(width, height, internalFormat, GL_RGBA, data_type, 0);

			m_ptr->attachments.push_back(texture);

			m_ptr->colors.push_back(texture.get());
		}

		// bind attachment [color_0] as default
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ptr->colors[0]->ID(), 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			Log::Message("[FrameBuffer] Framebuffer (color attachments) not complete!", Log::ERROR);
		}

		Unbind();
	}

	void FrameBuffer::GenerateDepthAttachment(unsigned int width, unsigned int height, unsigned int data_type, unsigned int num_attachment)
	{
		m_ptr->width = width;
		m_ptr->height = height;
		m_ptr->Generate();

		Bind();

		for (unsigned int i = 0; i < num_attachment; ++i)
		{
			std::shared_ptr<Texture> texture = std::make_shared<Texture>();

			texture->attribute.filterMin = GL_NEAREST;
			texture->attribute.filterMax = GL_NEAREST;
			texture->attribute.wrapS = GL_CLAMP_TO_EDGE;
			texture->attribute.wrapT = GL_CLAMP_TO_EDGE;
			texture->attribute.mipmapping = false;

			texture->Generate2D(width, height, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, data_type, 0);

			m_ptr->attachments.push_back(texture);

			m_ptr->depths.push_back(texture.get());
		}

		// bind attachment [depth_0] as default
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_ptr->depths[0]->ID(), 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			Log::Message("[FrameBuffer] Framebuffer (depth) not complete!", Log::ERROR);
		}

		Unbind();
	}

	void FrameBuffer::GenerateDepthRenderBuffer(unsigned int width, unsigned height)
	{
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
		m_ptr->width = width;
		m_ptr->height = height;
		m_ptr->Generate();

		Bind();

		std::shared_ptr<Texture> texture = std::make_shared<Texture>();

		texture->attribute.filterMin = GL_NEAREST;
		texture->attribute.filterMax = GL_NEAREST;
		texture->attribute.wrapS = GL_CLAMP_TO_EDGE;
		texture->attribute.wrapT = GL_CLAMP_TO_EDGE;
		texture->attribute.mipmapping = false;

		texture->Generate2D(width, height, GL_DEPTH_STENCIL, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture->ID(), 0);

		m_ptr->attachments.push_back(texture);

		m_ptr->depths.push_back(texture.get());

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			Log::Message("[FrameBuffer] Framebuffer (depth-stencil) not complete!", Log::ERROR);
		}

		Unbind();
	}

	void FrameBuffer::GenerateDepthStencilRenderBuffer(unsigned int width, unsigned height)
	{
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
		m_ptr->width = width;
		m_ptr->height = height;
		m_ptr->Generate();

		Bind();

		for (unsigned int i = 0; i < num_attachment; ++i)
		{
			std::shared_ptr<Texture> texture = std::make_shared<Texture>();

			texture->attribute.filterMin = GL_LINEAR;
			texture->attribute.filterMax = GL_LINEAR;
			texture->attribute.wrapS = GL_CLAMP_TO_EDGE;
			texture->attribute.wrapT = GL_CLAMP_TO_EDGE;
			texture->attribute.wrapR = GL_CLAMP_TO_EDGE;
			texture->attribute.mipmapping = false;

			texture->GenerateCube(width, height, GL_RGB, data_type, 0);

			m_ptr->attachments.push_back(texture);

			m_ptr->colors.push_back(texture.get());
		}

		// bind attachment [color_0], face [+X] as default
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, m_ptr->colors[0]->ID(), 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			Log::Message("[FrameBuffer] Framebuffer (cubic color attachments) not complete!", Log::ERROR);
		}

		Unbind();
	}

	void FrameBuffer::GenerateCubeMapDepthAttachment(unsigned int width, unsigned int height, unsigned int data_format, unsigned int num_attachment)
	{
		m_ptr->width = width;
		m_ptr->height = height;
		m_ptr->Generate();

		Bind();

		for (unsigned int i = 0; i < num_attachment; ++i)
		{
			std::shared_ptr<Texture> texture = std::make_shared<Texture>();

			texture->attribute.filterMin = GL_NEAREST;
			texture->attribute.filterMax = GL_NEAREST;
			texture->attribute.wrapS = GL_CLAMP_TO_EDGE;
			texture->attribute.wrapT = GL_CLAMP_TO_EDGE;
			texture->attribute.wrapR = GL_CLAMP_TO_EDGE;
			texture->attribute.mipmapping = false;

			texture->GenerateCube(width, height, GL_DEPTH_COMPONENT, data_format, 0);

			// require geometry shader support
			//glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture->ID(), 0);

			m_ptr->attachments.push_back(texture);

			m_ptr->depths.push_back(texture.get());
		}

		// bind attachment [depth_0], face [+X] as default
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X, m_ptr->depths[0]->ID(), 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			Log::Message("[FrameBuffer] Framebuffer (cubic depth) not complete!", Log::ERROR);
		}

		Unbind();
	}
}