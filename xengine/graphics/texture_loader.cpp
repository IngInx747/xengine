#include "texture_loader.h"

#include <vendor/glad/glad.h>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <vendor/stb_image/stb_image.h>
#endif

#include <utility/log.h>
#include <utility/file_system.h>

namespace xengine
{
	Texture LoadTexture2D_Impl_Stbi(const std::string& filename, unsigned int colorFormat, bool srgb)
	{
		Texture texture;
		int width, height, nrComponents;

		if (colorFormat == GL_RGB || colorFormat == GL_SRGB)
			colorFormat = srgb ? GL_SRGB : GL_RGB;
		if (colorFormat == GL_RGBA || colorFormat == GL_SRGB_ALPHA)
			colorFormat = srgb ? GL_SRGB_ALPHA : GL_RGBA;

		stbi_set_flip_vertically_on_load(true);
		unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

		if (data)
		{
			GLenum pixelFormat;
			if (nrComponents == 1) pixelFormat = GL_RED;
			else if (nrComponents == 3) pixelFormat = GL_RGB;
			else if (nrComponents == 4) pixelFormat = GL_RGBA;

			texture.Generate2D(width, height, colorFormat, pixelFormat, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			Log::Message("[TextureManager] Cannot load 2D texture \"" + filename + "\"", Log::WARN);
			stbi_image_free(data);
			return Texture();
		}

		return texture;
	}

	Texture LoadHDR_Impl_Stbi(const std::string& filename)
	{
		Texture texture;

		if (!stbi_is_hdr(filename.c_str()))
		{
			Log::Message("[TextureManager] File \"" + filename + "\" is not HDR format or does not exist", Log::WARN);
			return Texture();
		}

		int width, height, nrComponents;

		stbi_set_flip_vertically_on_load(true);
		float *data = stbi_loadf(filename.c_str(), &width, &height, &nrComponents, 0);

		if (data)
		{
			GLenum colorFormat, pixelFormat;

			if (nrComponents == 3)
			{
				colorFormat = GL_RGB32F;
				pixelFormat = GL_RGB;
			}
			else if (nrComponents == 4)
			{
				colorFormat = GL_RGBA32F;
				pixelFormat = GL_RGBA;
			}

			texture.Generate2D(width, height, colorFormat, pixelFormat, GL_FLOAT, data);
			texture.SetFilterMin(GL_LINEAR);
			stbi_image_free(data);
		}
		else
		{
			Log::Message("[TextureManager] Cannot load HDR texture \"" + filename + "\"", Log::WARN);
			stbi_image_free(data);
			return Texture();
		}

		return texture;
	}

	CubeMap LoadCubeMap_Impl_Stbi(
		const std::string & filenameTop,
		const std::string & filenameBottom,
		const std::string & filenameLeft,
		const std::string & filenameRight,
		const std::string & filenameFront,
		const std::string & filenameBack)
	{
		CubeMap texture;

		// disable y flip on cubemaps
		stbi_set_flip_vertically_on_load(false);

		// order:  +X (right), -X (left), +Y (top), -Y (bottom), +Z (front), -Z (back)
		std::vector<std::string> faces{ filenameRight, filenameLeft, filenameTop, filenameBottom, filenameFront, filenameBack };

		for (unsigned int i = 0; i < 6; ++i)
		{
			int width, height, nrComponents;
			unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);

			if (data)
			{
				GLenum format;
				if (nrComponents == 3) format = GL_RGB;
				else format = GL_RGBA;

				texture.GenerateCube(width, height, format, GL_UNSIGNED_BYTE, i, data);
				stbi_image_free(data);
			}
			else
			{
				Log::Message("[TextureManager] Cannot load cubic texture \"" + faces[i] + "\"", Log::WARN);
				stbi_image_free(data);
			}
		}

		if (texture.Mipmap()) glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		return texture;
	}

	CubeMap LoadCubeMap_Impl_Stbi(const std::string& folder)
	{
		if (!FileSystem::IsDirectory(folder))
		{
			Log::Message("[TextureManager] Cannot load cubic texture. Path \"" + folder + "\" is not a folder.", Log::WARN);
			return Texture();
		}

		std::string extn{};

		if (FileSystem::Exist(folder + "top.jpg"))
		{
			extn = ".jpg";
		}
		else if (FileSystem::Exist(folder + "top.png"))
		{
			extn = ".png";
		}
		else
		{
			Log::Message("[TextureManager] Cannot load cubic texture in folder \"" + folder + "\". Textures do not exist.", Log::WARN);
			return Texture();
		}

		return LoadCubeMap_Impl_Stbi(
			folder + "right" + extn,
			folder + "left" + extn,
			folder + "top" + extn,
			folder + "bottom" + extn,
			folder + "front" + extn,
			folder + "back" + extn);
	}
}