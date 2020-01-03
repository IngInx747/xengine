#include "texture.h"

#include <vendor/glad/glad.h>

#include <utility/log.h>

namespace xengine
{
	////////////////////////////////////////////////////////////////
	// Texture Unique Instance
	////////////////////////////////////////////////////////////////

	TextureMemory::TextureMemory()
		:
		SharedMemory()
	{
		target = GL_TEXTURE_2D;
		width = 0;
		height = 0;
		depth = 0;
		colorFormat = GL_RGBA;
		pixelFormat = GL_RGBA;
		dataType = GL_UNSIGNED_BYTE;
		filterMin = GL_LINEAR_MIPMAP_LINEAR;
		filterMax = GL_LINEAR;
		wrapS = GL_REPEAT;
		wrapT = GL_REPEAT;
		wrapR = GL_REPEAT;
		mipmapping = true;
	}

	TextureMemory::~TextureMemory()
	{
		Destory();
	}

	void TextureMemory::Generate()
	{
		if (!m_id)
		{
			glGenTextures(1, &m_id);
			Log::Message("[TextureMemory] Texture " + std::to_string(m_id) + " generated", Log::DEBUG);
		}
	}

	void TextureMemory::Destory()
	{
		if (m_id)
		{
			Log::Message("[TextureMemory] Texture " + std::to_string(m_id) + " deleted", Log::DEBUG);
			glDeleteTextures(1, &m_id);
			m_id = 0;
		}
	}
	
	////////////////////////////////////////////////////////////////
	// Texture
	////////////////////////////////////////////////////////////////

	Texture::Texture()
		:
		SharedHandle()
	{
	}

	Texture::~Texture()
	{
	}

	Texture::Texture(const Texture & other)
		:
		SharedHandle(other)
	{
		m_ptr = other.m_ptr;
	}

	Texture & Texture::operator=(const Texture & other)
	{
		this->SharedHandle::operator=(other);
		m_ptr = other.m_ptr;
		return *this;
	}

	void Texture::allocateMemory()
	{
		if (m_ptr) return;

		m_ptr = new TextureMemory;

		SharedHandle::Register(m_ptr);
	}

	void Texture::generateObject()
	{
		m_ptr->Generate();
	}

	void Texture::generate()
	{
		allocateMemory();
		generateObject();
	}

	void Texture::Bind(int unit)
	{
		if (unit >= 0) glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(m_ptr->target, m_ptr->m_id);
	}

	void Texture::Unbind()
	{
		glBindTexture(m_ptr->target, 0);
	}

	void Texture::Resize(unsigned int width, unsigned int height, unsigned int depth)
	{
		allocateMemory();

		if (m_ptr->m_id == 0) return;

		m_ptr->width = width;
		m_ptr->height = height;
		m_ptr->depth = depth;

		Bind();

		switch (m_ptr->target)
		{
		case GL_TEXTURE_1D:
			glTexImage1D(GL_TEXTURE_1D, 0, m_ptr->colorFormat, width, 0, m_ptr->pixelFormat, m_ptr->dataType, 0);
			break;
		case GL_TEXTURE_2D:
			glTexImage2D(GL_TEXTURE_2D, 0, m_ptr->colorFormat, width, height, 0, m_ptr->pixelFormat, m_ptr->dataType, 0);
			break;
		case GL_TEXTURE_3D:
			glTexImage3D(GL_TEXTURE_3D, 0, m_ptr->colorFormat, width, height, depth, 0, m_ptr->pixelFormat, m_ptr->dataType, 0);
			break;
		case GL_TEXTURE_CUBE_MAP:
			for (unsigned int i = 0; i < 6; ++i) glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_ptr->colorFormat, width, height, 0, m_ptr->pixelFormat, m_ptr->dataType, 0);
			break;
		default:
			break;
		}

		Unbind();
	}

	void Texture::Generate1D(unsigned int width, unsigned int colorFormat, unsigned int pixelFormat, unsigned int data_type, void * data)
	{
		generate();

		m_ptr->target = GL_TEXTURE_1D;
		m_ptr->width = width;
		m_ptr->colorFormat = colorFormat;
		m_ptr->pixelFormat = pixelFormat;
		m_ptr->dataType = data_type;

		Bind();
		glTexImage1D(m_ptr->target, 0, colorFormat, width, 0, pixelFormat, data_type, data);
		glTexParameteri(m_ptr->target, GL_TEXTURE_MIN_FILTER, m_ptr->filterMin);
		glTexParameteri(m_ptr->target, GL_TEXTURE_MAG_FILTER, m_ptr->filterMax);
		glTexParameteri(m_ptr->target, GL_TEXTURE_WRAP_S, m_ptr->wrapS);
		if (m_ptr->mipmapping) glGenerateMipmap(m_ptr->target);
		Unbind();
	}

	void Texture::Generate2D(unsigned int width, unsigned int height, unsigned int colorFormat, unsigned int pixelFormat, unsigned int data_type, void * data)
	{
		generate();

		m_ptr->target = GL_TEXTURE_2D;
		m_ptr->width = width;
		m_ptr->height = height;
		m_ptr->colorFormat = colorFormat;
		m_ptr->pixelFormat = pixelFormat;
		m_ptr->dataType = data_type;

		Bind();
		glTexImage2D(m_ptr->target, 0, colorFormat, width, height, 0, pixelFormat, data_type, data);
		glTexParameteri(m_ptr->target, GL_TEXTURE_MIN_FILTER, m_ptr->filterMin);
		glTexParameteri(m_ptr->target, GL_TEXTURE_MAG_FILTER, m_ptr->filterMax);
		glTexParameteri(m_ptr->target, GL_TEXTURE_WRAP_S, m_ptr->wrapS);
		glTexParameteri(m_ptr->target, GL_TEXTURE_WRAP_T, m_ptr->wrapT);
		if (m_ptr->mipmapping) glGenerateMipmap(m_ptr->target);
		Unbind();
	}

	void Texture::Generate3D(unsigned int width, unsigned int height, unsigned int depth, unsigned int colorFormat, unsigned int pixelFormat, unsigned int data_type, void * data)
	{
		generate();

		m_ptr->target = GL_TEXTURE_3D;
		m_ptr->height = height;
		m_ptr->depth = depth;
		m_ptr->colorFormat = colorFormat;
		m_ptr->pixelFormat = pixelFormat;
		m_ptr->dataType = data_type;
		m_ptr->width = width;

		Bind();
		glTexImage3D(m_ptr->target, 0, colorFormat, width, height, depth, 0, pixelFormat, data_type, data);
		glTexParameteri(m_ptr->target, GL_TEXTURE_MIN_FILTER, m_ptr->filterMin);
		glTexParameteri(m_ptr->target, GL_TEXTURE_MAG_FILTER, m_ptr->filterMax);
		glTexParameteri(m_ptr->target, GL_TEXTURE_WRAP_S, m_ptr->wrapS);
		glTexParameteri(m_ptr->target, GL_TEXTURE_WRAP_T, m_ptr->wrapT);
		glTexParameteri(m_ptr->target, GL_TEXTURE_WRAP_R, m_ptr->wrapR);
		if (m_ptr->mipmapping) glGenerateMipmap(m_ptr->target);
		Unbind();
	}

	void Texture::GenerateCube(unsigned int width, unsigned int height, unsigned int format, unsigned int data_type, bool mipmap)
	{
		generate();

		m_ptr->target = GL_TEXTURE_CUBE_MAP;
		m_ptr->width = width;
		m_ptr->height = height;
		m_ptr->colorFormat = format;
		m_ptr->pixelFormat = format;
		m_ptr->dataType = data_type;
		m_ptr->mipmapping = mipmap;

		if (data_type == GL_HALF_FLOAT && format == GL_RGB)
			m_ptr->colorFormat = GL_RGB16F;
		else if (data_type == GL_FLOAT && format == GL_RGB)
			m_ptr->colorFormat = GL_RGB32F;
		else if (data_type == GL_HALF_FLOAT && format == GL_RGBA)
			m_ptr->colorFormat = GL_RGBA16F;
		else if (data_type == GL_FLOAT && format == GL_RGBA)
			m_ptr->colorFormat = GL_RGBA32F;

		Bind();
		for (unsigned int i = 0; i < 6; ++i) glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_ptr->colorFormat, width, height, 0, format, data_type, 0);
		glTexParameteri(m_ptr->target, GL_TEXTURE_MIN_FILTER, m_ptr->filterMin);
		glTexParameteri(m_ptr->target, GL_TEXTURE_MAG_FILTER, m_ptr->filterMax);
		glTexParameteri(m_ptr->target, GL_TEXTURE_WRAP_S, m_ptr->wrapS);
		glTexParameteri(m_ptr->target, GL_TEXTURE_WRAP_T, m_ptr->wrapT);
		glTexParameteri(m_ptr->target, GL_TEXTURE_WRAP_R, m_ptr->wrapR);
		if (m_ptr->mipmapping) glGenerateMipmap(m_ptr->target);
		Unbind();
	}

	void Texture::GenerateCube(unsigned int width, unsigned int height, unsigned int format, unsigned int type, unsigned int face, unsigned char * data)
	{
		generate();

		m_ptr->target = GL_TEXTURE_CUBE_MAP;
		m_ptr->width = width;
		m_ptr->height = height;
		m_ptr->colorFormat = format;
		m_ptr->pixelFormat = format;
		m_ptr->dataType = type;

		Bind();
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, format, width, height, 0, format, type, data);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, m_ptr->filterMin);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, m_ptr->filterMax);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, m_ptr->wrapS);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, m_ptr->wrapT);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, m_ptr->wrapR);
		if (m_ptr->mipmapping) glGenerateMipmap(m_ptr->target);
		Unbind();
	}

	void Texture::SetFilterMin(unsigned int filter)
	{
		allocateMemory();

		m_ptr->filterMin = filter;

		if (m_ptr->m_id)
		{
			Bind();
			glTexParameteri(m_ptr->target, GL_TEXTURE_MIN_FILTER, filter);
		}
	}

	void Texture::SetFilterMax(unsigned int filter)
	{
		allocateMemory();

		m_ptr->filterMax = filter;

		if (m_ptr->m_id)
		{
			Bind();
			glTexParameteri(m_ptr->target, GL_TEXTURE_MAG_FILTER, filter);
		}
	}

	void Texture::SetMipmap(bool mipmap)
	{
		allocateMemory();

		m_ptr->mipmapping = mipmap;

		if (m_ptr->m_id)
		{
			if (mipmap)
				glGenerateMipmap(m_ptr->target);
		}
	}

	void Texture::SetWrapS(unsigned int wrapMode)
	{
		allocateMemory();

		m_ptr->wrapS = wrapMode;

		if (m_ptr->m_id)
		{
			Bind();
			glTexParameteri(m_ptr->target, GL_TEXTURE_WRAP_S, wrapMode);
		}
	}

	void Texture::SetWrapT(unsigned int wrapMode)
	{
		allocateMemory();

		m_ptr->wrapT = wrapMode;

		if (m_ptr->m_id)
		{
			Bind();
			glTexParameteri(m_ptr->target, GL_TEXTURE_WRAP_T, wrapMode);
		}
	}

	void Texture::SetWrapR(unsigned int wrapMode)
	{
		allocateMemory();

		m_ptr->wrapR = wrapMode;

		if (m_ptr->m_id)
		{
			Bind();
			glTexParameteri(m_ptr->target, GL_TEXTURE_WRAP_R, wrapMode);
		}
	}

	void Texture::SetWrapSTR(unsigned int wrapMode)
	{
		allocateMemory();

		switch (m_ptr->target)
		{
		case GL_TEXTURE_1D:
			SetWrapS(wrapMode);
			break;
		case GL_TEXTURE_2D:
			SetWrapS(wrapMode);
			SetWrapT(wrapMode);
			break;
		case GL_TEXTURE_3D:
			SetWrapS(wrapMode);
			SetWrapT(wrapMode);
			SetWrapR(wrapMode);
			break;
		case GL_TEXTURE_CUBE_MAP:
			SetWrapS(wrapMode);
			SetWrapT(wrapMode);
			SetWrapR(wrapMode);
			break;
		default:
			break;
		}
	}
}