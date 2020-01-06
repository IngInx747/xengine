#include "texture_manager.h"

#include <vendor/glad/glad.h>

#include <utility/log.h>
#include <utility/file_system.h>

#include "texture_loader.h"

namespace xengine
{
	std::unordered_map<std::string, Texture> TextureManager::g_localTable{};
	std::unordered_map<std::string, Texture> TextureManager::g_globalTable{};
	Texture TextureManager::_nullTexture2D;

	void TextureManager::Initialize()
	{
		generateDefaultTexture();

		_nullTexture2D = g_globalTable["chessboard"];
	}

	void TextureManager::Clear()
	{
		ClearLocal();
		ClearGlobal();
	}

	void TextureManager::ClearLocal()
	{
		g_localTable.clear();
	}

	void TextureManager::ClearGlobal()
	{
		g_globalTable.clear();
	}

	Texture TextureManager::Get(const std::string& name)
	{
		// search for scene-specific resources
		{
			auto it = g_localTable.find(name);
			if (it != g_localTable.end()) return it->second;
		}

		// if texture was not loaded along with the scene, search for default resources
		{
			auto it = g_globalTable.find(name);
			if (it != g_globalTable.end()) return it->second;
		}

		Log::Message("[TextureManager] Texture \"" + name + "\" not found", Log::WARN);
		return _nullTexture2D;
	}

	Texture TextureManager::LoadTexture2D(const std::string& name, const std::string& path, unsigned int format, bool srgb)
	{
		// if texture exists in scene-specific resources, return it directly
		auto it = g_localTable.find(name);
		if (it != g_localTable.end()) return it->second;

		// Note: Here we do not search in default resources, so the texture can override
		// the default texture, which possibly already exists and has the same name.

		Log::Message("[TextureManager] Loading 2D texture \"" + name + "\" from \"" + path + "\" ...", Log::INFO);

		Texture texture = LoadTexture2D_Impl_Stbi(path, format, srgb);

		if (!texture)
		{
			Log::Message("[TextureManager] 2D Texture \"" + name + "\" loading failed", Log::WARN);
			return _nullTexture2D;
		}

		g_localTable[name] = texture;

		Log::Message("[TextureManager] 2D Texture \"" + name + "\" loaded successfully", Log::INFO);

		return texture;
	}

	Texture TextureManager::LoadHDR(const std::string& name, const std::string& path)
	{
		auto it = g_localTable.find(name);
		if (it != g_localTable.end()) return it->second;

		Log::Message("[TextureManager] Loading HDR texture \"" + name + "\" from \"" + path + "\" ...", Log::INFO);

		Texture texture = LoadHDR_Impl_Stbi(path);

		if (!texture)
		{
			Log::Message("[TextureManager] HDR texture \"" + name + "\" loading failed", Log::WARN);
			return _nullTexture2D;
		}

		g_localTable[name] = texture;

		Log::Message("[TextureManager] HDR texture \"" + name + "\" loaded successfully", Log::INFO);

		return texture;
	}

	CubeMap TextureManager::LoadCubeMap(const std::string& name, const std::string& directory)
	{
		auto it = g_localTable.find(name);
		if (it != g_localTable.end()) return it->second;

		Log::Message("[TextureManager] Loading cube map \"" + name + "\" from \"" + directory + "\" ...", Log::INFO);

		CubeMap texture = LoadCubeMap_Impl_Stbi(directory);

		if (!texture)
		{
			Log::Message("[TextureManager] Cube map \"" + name + "\" loading failed", Log::WARN);
			return _nullTexture2D; // TODO: use default cube map here
		}

		g_localTable[name] = texture;

		Log::Message("[TextureManager] Cube map \"" + name + "\" loaded successfully", Log::INFO);

		return texture;
	}

	Texture TextureManager::CreateTexture2DPureColor(
		unsigned int colorFormat,
		unsigned int pixelFormat,
		unsigned int width,
		unsigned int height,
		unsigned char color[4])
	{
		Texture texture;

		struct Pixel { unsigned char channel[4]; } pixel{ color[0], color[1], color[2], color[3] };
		std::vector<Pixel> data(width * height, pixel);

		texture.Generate2D(width, height, colorFormat, pixelFormat, GL_UNSIGNED_BYTE, &color[0]);

		return texture;
	}

	Texture TextureManager::CreateTexture2DChessboard(
		unsigned int colorFormat,
		unsigned int pixelFormat,
		unsigned int width,
		unsigned int height,
		unsigned char color1[4],
		unsigned char color2[4])
	{
		Texture texture;

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

		texture.Generate2D(width, height, colorFormat, pixelFormat, GL_UNSIGNED_BYTE, &data[0]);
		texture.SetFilterMin(GL_NEAREST);
		texture.SetFilterMax(GL_NEAREST);

		return texture;
	}

	void TextureManager::generateDefaultTexture()
	{
		{
			unsigned char color[4]{ 255, 255, 255, 255 };
			Texture texture = CreateTexture2DPureColor(GL_RGBA, GL_RGBA, 1, 1, color);
			g_globalTable["white"] = texture;
		}

		{
			unsigned char color[4]{   1,   1,   1, 255 };
			Texture texture = CreateTexture2DPureColor(GL_RGBA, GL_RGBA, 1, 1, color);
			g_globalTable["black"] = texture;
		}

		{
			unsigned char color[4]{ 128, 128, 255, 255 };
			Texture texture = CreateTexture2DPureColor(GL_RGBA, GL_RGBA, 1, 1, color);
			g_globalTable["normal"] = texture;
		}

		{
			unsigned char color1[4]{ 255, 255, 255, 255 };
			unsigned char color2[4]{   1,   1,   1, 255 };
			Texture texture = CreateTexture2DChessboard(GL_RGBA, GL_RGBA, 8, 8, color1, color2);
			g_globalTable["chessboard"] = texture;
		}
	}
}