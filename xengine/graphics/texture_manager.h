#pragma once
#ifndef XE_TEXTURE_MANAGER_H
#define XE_TEXTURE_MANAGER_H

#include "texture.h"

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

namespace xengine
{
	class TextureManager
	{
	public:
		static void Initialize();
		static void Clear();
		static void ClearScene();
		static void ClearDefault();

		// get a named texture (if not exist, search for default resources)
		static Texture* Get(const std::string& name);

		// load a 1D texture
		// TODO

		// load a 2D texture
		static Texture* LoadTexture2D(
			const std::string& name,
			const std::string& path,
			unsigned int format,
			bool srgb = false);

		// load a 3D texture
		// TODO

		// load a high-dynamical-range texture
		static Texture* LoadHDR(const std::string& name, const std::string& path);

		// load a cubic texture
		static CubeMap* LoadCubeMap(const std::string& name, const std::string& directory);

		// create a pure color texture
		static std::shared_ptr<Texture> CreateTexture2DPureColor(
			unsigned int colorFormat,
			unsigned int pixelFormat,
			unsigned int width,
			unsigned int height,
			unsigned char color[4]);

		// create a 2-color chessboard texture
		static std::shared_ptr<Texture> CreateTexture2DChessboard(
			unsigned int colorFormat,
			unsigned int pixelFormat,
			unsigned int width,
			unsigned int height,
			unsigned char color1[4],
			unsigned char color2[4]);

	private:
		// load a 2D texture
		static std::shared_ptr<Texture> loadTexture2D(const std::string& filename, unsigned int colorFormat, bool srgb);

		// load a high-dynamical-range texture
		static std::shared_ptr<Texture> loadHDR(const std::string& filename);

		// load a cubic texture from specific files
		static std::shared_ptr<CubeMap> loadCubeMap(
			const std::string& filenameTop,
			const std::string& filenameBottom,
			const std::string& filenameLeft,
			const std::string& filenameRight,
			const std::string& filenameFront,
			const std::string& filenameBack);

		// load a cubic texture from folder (filenames should be named "right", "top", etc., to specify facing direction)
		static std::shared_ptr<CubeMap> loadCubeMap(const std::string& folder);

		static void generateDefaultTexture();

	private:
		// resources containers
		static std::vector<std::shared_ptr<Texture>> _textures;

		// lookup tables
		static std::unordered_map<std::string, Texture*> _textureTable;

		// default texture
		static std::vector<std::shared_ptr<Texture>> _defaultTextures;

		// lookup tables
		static std::unordered_map<std::string, Texture*> _defaultTextureTable;

		// null protector (if a texture fails to load, this texture will be the output)
		static Texture* _nullTexture2D;
		// TODO: more types of protectors
	};
}

#endif // !XE_TEXTURE_MANAGER_H
