#include "texture_manager.h"

#include <vendor/glad/glad.h>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <vendor/stb_image/stb_image.h>
#endif

#include <utility/log.h>
#include <utility/file_system.h>

namespace xengine
{
	std::vector<std::shared_ptr<Texture>> TextureManager::_textures{};
	std::unordered_map<std::string, Texture*> TextureManager::_textureTable{};

	std::vector<std::shared_ptr<Texture>> TextureManager::_defaultTextures{};
	std::unordered_map<std::string, Texture*> TextureManager::_defaultTextureTable{};

	Texture* TextureManager::_nullTexture2D = nullptr;

	void TextureManager::Initialize()
	{
		generateDefaultTexture();

		_nullTexture2D = _defaultTextureTable["chessboard"];
	}

	void TextureManager::Clear()
	{
		ClearScene();
		ClearDefault();
	}

	void TextureManager::ClearScene()
	{
		_textureTable.clear();
		_textures.clear();
	}

	void TextureManager::ClearDefault()
	{
		_defaultTextureTable.clear();
		_defaultTextures.clear();
	}

	Texture* TextureManager::Get(const std::string& name)
	{
		// search for scene-specific resources
		{
			auto it = _textureTable.find(name);
			if (it != _textureTable.end()) return it->second;
		}

		// if texture was not loaded along with the scene, search for default resources
		{
			auto it = _defaultTextureTable.find(name);
			if (it != _defaultTextureTable.end()) return it->second;
		}

		Log::Message("[TextureManager] Texture \"" + name + "\" not found", Log::WARN);
		return _nullTexture2D;
	}

	Texture* TextureManager::LoadTexture2D(const std::string& name, const std::string& path, unsigned int format, bool srgb)
	{
		// if texture exists in scene-specific resources, return it directly
		auto it = _textureTable.find(name);
		if (it != _textureTable.end()) return it->second;

		// Note: Here we do not search in default resources, so the texture can override
		// the default texture, which possibly already exists and has the same name.

		Log::Message("[TextureManager] Loading 2D texture \"" + name + "\" from \"" + path + "\" ...", Log::INFO);

		std::shared_ptr<Texture> texture = loadTexture2D(path, format, srgb);

		if (!texture)
		{
			Log::Message("[TextureManager] 2D Texture \"" + name + "\" loading failed", Log::WARN);
			return _nullTexture2D;
		}

		_textures.push_back(texture);
		_textureTable[name] = texture.get();

		Log::Message("[TextureManager] 2D Texture \"" + name + "\" loaded successfully", Log::INFO);

		return texture.get();
	}

	Texture* TextureManager::LoadHDR(const std::string& name, const std::string& path)
	{
		auto it = _textureTable.find(name);
		if (it != _textureTable.end()) return it->second;

		Log::Message("[TextureManager] Loading HDR texture \"" + name + "\" from \"" + path + "\" ...", Log::INFO);

		std::shared_ptr<Texture> texture = loadHDR(path);

		if (!texture)
		{
			Log::Message("[TextureManager] HDR texture \"" + name + "\" loading failed", Log::WARN);
			return _nullTexture2D;
		}

		_textures.push_back(texture);
		_textureTable[name] = texture.get();

		Log::Message("[TextureManager] HDR texture \"" + name + "\" loaded successfully", Log::INFO);

		return texture.get();
	}

	CubeMap* TextureManager::LoadCubeMap(const std::string& name, const std::string& directory)
	{
		auto it = _textureTable.find(name);
		if (it != _textureTable.end()) return it->second;

		Log::Message("[TextureManager] Loading cube map \"" + name + "\" from \"" + directory + "\" ...", Log::INFO);

		std::shared_ptr<CubeMap> texture = loadCubeMap(directory);

		if (!texture)
		{
			Log::Message("[TextureManager] Cube map \"" + name + "\" loading failed", Log::WARN);
			return nullptr;
		}

		_textures.push_back(texture);
		_textureTable[name] = texture.get();

		Log::Message("[TextureManager] Cube map \"" + name + "\" loaded successfully", Log::INFO);

		return texture.get();
	}

	std::shared_ptr<Texture> TextureManager::loadTexture2D(const std::string& filename, unsigned int colorFormat, bool srgb)
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
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

			texture->Generate2D(width, height, colorFormat, pixelFormat, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			Log::Message("[TextureManager] Cannot load 2D texture \"" + filename + "\"", Log::WARN);
			stbi_image_free(data);
			return nullptr;
		}

		return texture;
	}

	std::shared_ptr<Texture> TextureManager::loadHDR(const std::string& filename)
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->attribute.filterMin = GL_LINEAR;
		texture->attribute.mipmapping = false;

		if (!stbi_is_hdr(filename.c_str()))
		{
			Log::Message("[TextureManager] File \"" + filename + "\" is not HDR format or does not exist", Log::WARN);
			return nullptr;
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

			texture->Generate2D(width, height, colorFormat, pixelFormat, GL_FLOAT, data);
			stbi_image_free(data);
		}
		else
		{
			Log::Message("[TextureManager] Cannot load HDR texture \"" + filename + "\"", Log::WARN);
			stbi_image_free(data);
			return nullptr;
		}

		return texture;
	}

	std::shared_ptr<CubeMap> TextureManager::loadCubeMap(
		const std::string & filenameTop,
		const std::string & filenameBottom,
		const std::string & filenameLeft,
		const std::string & filenameRight,
		const std::string & filenameFront,
		const std::string & filenameBack)
	{
		std::shared_ptr<CubeMap> texture = std::make_shared<CubeMap>();

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

				texture->GenerateCube(width, height, format, GL_UNSIGNED_BYTE, i, data);
				stbi_image_free(data);
			}
			else
			{
				Log::Message("[TextureManager] Cannot load cubic texture \"" + faces[i] + "\"", Log::WARN);
				stbi_image_free(data);
			}
		}

		if (texture->attribute.mipmapping) glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		return texture;
	}

	std::shared_ptr<CubeMap> TextureManager::loadCubeMap(const std::string& folder)
	{
		if (!FileSystem::IsDirectory(folder))
		{
			Log::Message("[TextureManager] Cannot load cubic texture. Path \"" + folder + "\" is not a folder.", Log::WARN);
			return nullptr;
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
			return nullptr;
		}

		return loadCubeMap(
			folder + "right" + extn,
			folder + "left" + extn,
			folder + "top" + extn,
			folder + "bottom" + extn,
			folder + "front" + extn,
			folder + "back" + extn);
	}

	std::shared_ptr<Texture> TextureManager::CreateTexture2DPureColor(
		unsigned int colorFormat,
		unsigned int pixelFormat,
		unsigned int width,
		unsigned int height,
		unsigned char color[4])
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();

		struct Pixel { unsigned char channel[4]; } pixel{ color[0], color[1], color[2], color[3] };
		std::vector<Pixel> data(width * height, pixel);

		texture->Generate2D(width, height, colorFormat, pixelFormat, GL_UNSIGNED_BYTE, &color[0]);

		return texture;
	}

	std::shared_ptr<Texture> TextureManager::CreateTexture2DChessboard(
		unsigned int colorFormat,
		unsigned int pixelFormat,
		unsigned int width,
		unsigned int height,
		unsigned char color1[4],
		unsigned char color2[4])
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();

		struct Pixel { unsigned char channel[4]; };
		Pixel pixel1{ color1[0], color1[1], color1[2], color1[3] };
		Pixel pixel2{ color2[0], color2[1], color2[2], color2[3] };

		std::vector<Pixel> data(width * height);

		for (unsigned int i = 0; i < height; ++i)
		{
			for (unsigned int j = 0; j < width; ++j)
			{
				data[i * width + j] = (i + j) % 2 ? pixel1 : pixel2;
			}
		}

		texture->SetFilterMin(GL_NEAREST);
		texture->SetFilterMax(GL_NEAREST);

		texture->Generate2D(width, height, colorFormat, pixelFormat, GL_UNSIGNED_BYTE, &data[0]);

		return texture;
	}

	void TextureManager::generateDefaultTexture()
	{
		{
			unsigned char color[4]{ 255, 255, 255, 255 };
			std::shared_ptr<Texture> texture = CreateTexture2DPureColor(GL_RGBA, GL_RGBA, 1, 1, color);
			_defaultTextures.push_back(texture);
			_defaultTextureTable["white"] = texture.get();
		}

		{
			unsigned char color[4]{   1,   1,   1, 255 };
			std::shared_ptr<Texture> texture = CreateTexture2DPureColor(GL_RGBA, GL_RGBA, 1, 1, color);
			_defaultTextures.push_back(texture);
			_defaultTextureTable["black"] = texture.get();
		}

		{
			unsigned char color[4]{ 128, 128, 255, 255 };
			std::shared_ptr<Texture> texture = CreateTexture2DPureColor(GL_RGBA, GL_RGBA, 1, 1, color);
			_defaultTextures.push_back(texture);
			_defaultTextureTable["normal"] = texture.get();
		}

		{
			unsigned char color1[4]{ 255, 255, 255, 255 };
			unsigned char color2[4]{   1,   1,   1, 255 };
			std::shared_ptr<Texture> texture = CreateTexture2DChessboard(GL_RGBA, GL_RGBA, 8, 8, color1, color2);
			_defaultTextures.push_back(texture);
			_defaultTextureTable["chessboard"] = texture.get();
		}
	}
}