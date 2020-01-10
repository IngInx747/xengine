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
		// initialize shader manager (load default resources)
		static void Initialize();

		// clear resources
		static void Clear();

		// clear local resources (loaded with scene)
		static void ClearLocal();

		// clear global resources (default resources or shared by multiple scenes)
		static void ClearGlobal();

		// get a named texture (if not exist, search for default resources)
		static Texture Get(const std::string& name);

		// load a 1D texture
		// TODO

		// load a 2D texture
		static Texture LoadLocalTexture2D(
			const std::string& name,
			const std::string& path,
			unsigned int format,
			bool srgb = false);

		static Texture LoadGlobalTexture2D(
			const std::string& name,
			const std::string& path,
			unsigned int format,
			bool srgb = false);

		// load a 3D texture
		// TODO

		// load a high-dynamical-range texture
		static Texture LoadLocalTextureHDR(const std::string& name, const std::string& path);
		static Texture LoadGlobalTextureHDR(const std::string& name, const std::string& path);

		// load a cubic texture
		static CubeMap LoadLocalCubeMap(const std::string& name, const std::string& directory);
		static CubeMap LoadGlobalCubeMap(const std::string& name, const std::string& directory);

		// create a pure color texture
		static Texture CreateTexture2DPureColor(
			unsigned int colorFormat,
			unsigned int pixelFormat,
			unsigned int width,
			unsigned int height,
			unsigned char color[4]);

		// create a 2-color chessboard texture
		static Texture CreateTexture2DChessboard(
			unsigned int colorFormat,
			unsigned int pixelFormat,
			unsigned int width,
			unsigned int height,
			unsigned char color1[4],
			unsigned char color2[4]);

	private:
		// load a 2D texture
		static Texture loadTexture2D(
			std::unordered_map<std::string, Texture>& table,
			const std::string& name,
			const std::string& path,
			unsigned int format,
			bool srgb = false);

		// load a high-dynamical-range texture
		static Texture loadTextureHDR(
			std::unordered_map<std::string, Texture>& table,
			const std::string& name,
			const std::string& path);

		// load a cubic texture
		static CubeMap loadCubeMap(
			std::unordered_map<std::string, Texture>& table,
			const std::string& name,
			const std::string& directory);

		static void generateDefaultTexture();

	private:
		// lookup tables
		static std::unordered_map<std::string, Texture> g_localTable;

		// lookup tables
		static std::unordered_map<std::string, Texture> g_globalTable;

		// null protector (if a texture fails to load, this texture will be the output)
		static Texture _nullTexture2D;
		// TODO: more types of protectors
	};
}

#endif // !XE_TEXTURE_MANAGER_H
