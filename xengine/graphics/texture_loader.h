#pragma once
#ifndef XE_TEXTURE_LOADER_H
#define XE_TEXTURE_LOADER_H

#include <string>
#include <vector>

#include "texture.h"

namespace xengine
{
	// load a 2D texture with stb_image
	Texture LoadTexture2D_Impl_Stbi(const std::string& filename, unsigned int colorFormat, bool srgb);

	// load a high-dynamical-range texture
	Texture LoadHDR_Impl_Stbi(const std::string& filename);

	// load a cubic texture from specific files
	CubeMap LoadCubeMap_Impl_Stbi(
		const std::string& filenameTop,
		const std::string& filenameBottom,
		const std::string& filenameLeft,
		const std::string& filenameRight,
		const std::string& filenameFront,
		const std::string& filenameBack);

	// load a cubic texture from folder (filenames should specify facing direction)
	// in the order: right left top bottom front back
	CubeMap LoadCubeMap_Impl_Stbi(const std::string& folder);
}

#endif // !XE_TEXTURE_LOADER_H
