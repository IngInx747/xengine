#include "texture.h"

#include <unordered_map>

#include <vendor/glad/glad.h>

namespace xengine
{
	////////////////////////////////////////////////////////////////
	// Texture attribute
	////////////////////////////////////////////////////////////////

	Texture::Attribute::Attribute()
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
	
	////////////////////////////////////////////////////////////////
	// Texture
	////////////////////////////////////////////////////////////////

	Texture::Texture()
	{
	}

	Texture::~Texture()
	{
		DeleteGpuData();
	}

	void Texture::Bind(int unit)
	{
		if (unit >= 0) glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(attribute.target, m_id);
	}

	void Texture::Unbind()
	{
		glBindTexture(attribute.target, 0);
	}

	void Texture::Resize(unsigned int width, unsigned int height, unsigned int depth)
	{
		Bind();

		attribute.width = width;
		attribute.height = height;
		attribute.depth = depth;

		switch (attribute.target)
		{
		case GL_TEXTURE_1D:
			glTexImage1D(GL_TEXTURE_1D, 0, attribute.colorFormat, width, 0, attribute.pixelFormat, attribute.dataType, 0);
			break;
		case GL_TEXTURE_2D:
			glTexImage2D(GL_TEXTURE_2D, 0, attribute.colorFormat, width, height, 0, attribute.pixelFormat, attribute.dataType, 0);
			break;
		case GL_TEXTURE_3D:
			glTexImage3D(GL_TEXTURE_3D, 0, attribute.colorFormat, width, height, depth, 0, attribute.pixelFormat, attribute.dataType, 0);
			break;
		case GL_TEXTURE_CUBE_MAP:
			for (unsigned int i = 0; i < 6; ++i) glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, attribute.colorFormat, width, height, 0, attribute.pixelFormat, attribute.dataType, 0);
			break;
		default:
			break;
		}

		Unbind();
	}

	void Texture::DeleteGpuData()
	{
		if (m_id)
		{
			glDeleteTextures(1, &m_id);
			m_id = 0;
		}
	}

	void Texture::Generate1D(unsigned int width, unsigned int colorFormat, unsigned int pixelFormat, unsigned int data_type, void * data)
	{
		if (!m_id) glGenTextures(1, &m_id);

		attribute.target = GL_TEXTURE_1D;
		attribute.width = width;
		attribute.colorFormat = colorFormat;
		attribute.pixelFormat = pixelFormat;
		attribute.dataType = data_type;

		Bind();
		glTexImage1D(attribute.target, 0, colorFormat, width, 0, pixelFormat, data_type, data);
		glTexParameteri(attribute.target, GL_TEXTURE_MIN_FILTER, attribute.filterMin);
		glTexParameteri(attribute.target, GL_TEXTURE_MAG_FILTER, attribute.filterMax);
		glTexParameteri(attribute.target, GL_TEXTURE_WRAP_S, attribute.wrapS);
		if (attribute.mipmapping) glGenerateMipmap(attribute.target);
		Unbind();
	}

	void Texture::Generate2D(unsigned int width, unsigned int height, unsigned int colorFormat, unsigned int pixelFormat, unsigned int data_type, void * data)
	{
		if (!m_id) glGenTextures(1, &m_id);

		attribute.target = GL_TEXTURE_2D;
		attribute.width = width;
		attribute.height = height;
		attribute.colorFormat = colorFormat;
		attribute.pixelFormat = pixelFormat;
		attribute.dataType = data_type;

		Bind();
		glTexImage2D(attribute.target, 0, colorFormat, width, height, 0, pixelFormat, data_type, data);
		glTexParameteri(attribute.target, GL_TEXTURE_MIN_FILTER, attribute.filterMin);
		glTexParameteri(attribute.target, GL_TEXTURE_MAG_FILTER, attribute.filterMax);
		glTexParameteri(attribute.target, GL_TEXTURE_WRAP_S, attribute.wrapS);
		glTexParameteri(attribute.target, GL_TEXTURE_WRAP_T, attribute.wrapT);
		if (attribute.mipmapping) glGenerateMipmap(attribute.target);
		Unbind();
	}

	void Texture::Generate3D(unsigned int width, unsigned int height, unsigned int depth, unsigned int colorFormat, unsigned int pixelFormat, unsigned int data_type, void * data)
	{
		if (!m_id) glGenTextures(1, &m_id);

		attribute.target = GL_TEXTURE_3D;
		attribute.height = height;
		attribute.depth = depth;
		attribute.colorFormat = colorFormat;
		attribute.pixelFormat = pixelFormat;
		attribute.dataType = data_type;
		attribute.width = width;

		Bind();
		glTexImage3D(attribute.target, 0, colorFormat, width, height, depth, 0, pixelFormat, data_type, data);
		glTexParameteri(attribute.target, GL_TEXTURE_MIN_FILTER, attribute.filterMin);
		glTexParameteri(attribute.target, GL_TEXTURE_MAG_FILTER, attribute.filterMax);
		glTexParameteri(attribute.target, GL_TEXTURE_WRAP_S, attribute.wrapS);
		glTexParameteri(attribute.target, GL_TEXTURE_WRAP_T, attribute.wrapT);
		glTexParameteri(attribute.target, GL_TEXTURE_WRAP_R, attribute.wrapR);
		if (attribute.mipmapping) glGenerateMipmap(attribute.target);
		Unbind();
	}

	void Texture::GenerateCube(unsigned int width, unsigned int height, unsigned int format, unsigned int data_type, bool mipmap)
	{
		if (!m_id) glGenTextures(1, &m_id);

		attribute.target = GL_TEXTURE_CUBE_MAP;
		attribute.width = width;
		attribute.height = height;
		attribute.colorFormat = format;
		attribute.pixelFormat = format;
		attribute.dataType = data_type;
		attribute.mipmapping = mipmap;

		if (data_type == GL_HALF_FLOAT && format == GL_RGB)
			attribute.colorFormat = GL_RGB16F;
		else if (data_type == GL_FLOAT && format == GL_RGB)
			attribute.colorFormat = GL_RGB32F;
		else if (data_type == GL_HALF_FLOAT && format == GL_RGBA)
			attribute.colorFormat = GL_RGBA16F;
		else if (data_type == GL_FLOAT && format == GL_RGBA)
			attribute.colorFormat = GL_RGBA32F;

		Bind();
		for (unsigned int i = 0; i < 6; ++i) glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, attribute.colorFormat, width, height, 0, format, data_type, 0);
		glTexParameteri(attribute.target, GL_TEXTURE_MIN_FILTER, attribute.filterMin);
		glTexParameteri(attribute.target, GL_TEXTURE_MAG_FILTER, attribute.filterMax);
		glTexParameteri(attribute.target, GL_TEXTURE_WRAP_S, attribute.wrapS);
		glTexParameteri(attribute.target, GL_TEXTURE_WRAP_T, attribute.wrapT);
		glTexParameteri(attribute.target, GL_TEXTURE_WRAP_R, attribute.wrapR);
		if (attribute.mipmapping) glGenerateMipmap(attribute.target);
		Unbind();
	}

	void Texture::GenerateCube(unsigned int width, unsigned int height, unsigned int format, unsigned int type, unsigned int face, unsigned char * data)
	{
		if (!m_id) glGenTextures(1, &m_id);

		attribute.target = GL_TEXTURE_CUBE_MAP;
		attribute.width = width;
		attribute.height = height;
		attribute.colorFormat = format;
		attribute.pixelFormat = format;
		attribute.dataType = type;

		Bind();
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, format, width, height, 0, format, type, data);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, attribute.filterMin);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, attribute.filterMax);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, attribute.wrapS);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, attribute.wrapT);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, attribute.wrapR);
		if (attribute.mipmapping) glGenerateMipmap(attribute.target);
		Unbind();
	}

	void Texture::SetFilterMin(unsigned int filter)
	{
		Bind();
		attribute.filterMin = filter;
		glTexParameteri(attribute.target, GL_TEXTURE_MIN_FILTER, filter);
	}

	void Texture::SetFilterMax(unsigned int filter)
	{
		Bind();
		attribute.filterMax = filter;
		glTexParameteri(attribute.target, GL_TEXTURE_MAG_FILTER, filter);
	}

	void Texture::EnableMipmap()
	{
		Bind();
		attribute.mipmapping = true;
		glGenerateMipmap(attribute.target);
	}

	void Texture::SetWrapMode(unsigned int wrapMode)
	{
		Bind();

		switch (attribute.target)
		{
		case GL_TEXTURE_1D:
			attribute.wrapS = wrapMode;
			glTexParameteri(attribute.target, GL_TEXTURE_WRAP_S, wrapMode);
			break;
		case GL_TEXTURE_2D:
			attribute.wrapS = wrapMode;
			attribute.wrapT = wrapMode;
			glTexParameteri(attribute.target, GL_TEXTURE_WRAP_S, wrapMode);
			glTexParameteri(attribute.target, GL_TEXTURE_WRAP_T, wrapMode);
			break;
		case GL_TEXTURE_3D:
			attribute.wrapS = wrapMode;
			attribute.wrapT = wrapMode;
			attribute.wrapR = wrapMode;
			glTexParameteri(attribute.target, GL_TEXTURE_WRAP_S, wrapMode);
			glTexParameteri(attribute.target, GL_TEXTURE_WRAP_T, wrapMode);
			glTexParameteri(attribute.target, GL_TEXTURE_WRAP_R, wrapMode);
			break;
		case GL_TEXTURE_CUBE_MAP:
			attribute.wrapS = wrapMode;
			attribute.wrapT = wrapMode;
			attribute.wrapR = wrapMode;
			glTexParameteri(attribute.target, GL_TEXTURE_WRAP_S, wrapMode);
			glTexParameteri(attribute.target, GL_TEXTURE_WRAP_T, wrapMode);
			glTexParameteri(attribute.target, GL_TEXTURE_WRAP_R, wrapMode);
			break;
		default:
			break;
		}
	}
}