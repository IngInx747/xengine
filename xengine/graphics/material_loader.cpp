#include "material_loader.h"

#include <vendor/glad/glad.h>

#include <vendor/assimp/scene.h>
#include <vendor/assimp/Importer.hpp>
#include <vendor/assimp/postprocess.h>

#include "texture_manager.h"
#include "material_manager.h"

namespace xengine
{
	static std::string processPath(aiString* aPath, const std::string& directory)
	{
		std::string path = std::string(aPath->C_Str());

		// parse path directly if path contains "/" indicating it is an absolute path
		// otherwise parse as relative.
		if (path.find(":/") == std::string::npos || path.find(":\\") == std::string::npos)
			path = directory + "/" + path;

		return path;
	}

	Material LoadMaterial_Impl_Assimp(aiMaterial * aMaterial, const std::string & directory, aiMesh * aMesh)
	{
		Material material;
		aiString aString;
		aMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &aString);
		std::string texDiffPath{ aString.C_Str() };
		bool alpha = false;

		if (texDiffPath.find("_alpha") != std::string::npos)
		{
			material = MaterialManager::Get("alpha discard");
			alpha = true;
		}
		else
		{
			if (aMesh->HasTangentsAndBitangents())
				material = MaterialManager::Get("deferred");
			else
				material = MaterialManager::Get("deferred no TBN");
		}

		if (aMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			aiString aPath;
			aMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &aPath);
			std::string filename = processPath(&aPath, directory);

			Texture texture = TextureManager::LoadLocalTexture2D(filename, filename, alpha ? GL_RGBA : GL_RGB, true);
			if (texture) material.RegisterTexture("TexAlbedo", texture);
		}

		if (aMaterial->GetTextureCount(aiTextureType_DISPLACEMENT) > 0)
		{
			aiString aPath;
			aMaterial->GetTexture(aiTextureType_DISPLACEMENT, 0, &aPath);
			std::string filename = processPath(&aPath, directory);

			Texture texture = TextureManager::LoadLocalTexture2D(filename, filename, GL_RGBA, false);
			if (texture) material.RegisterTexture("TexNormal", texture);
		}

		if (aMaterial->GetTextureCount(aiTextureType_SPECULAR) > 0)
		{
			aiString aPath;
			aMaterial->GetTexture(aiTextureType_SPECULAR, 0, &aPath);
			std::string filename = processPath(&aPath, directory);

			Texture texture = TextureManager::LoadLocalTexture2D(filename, filename, GL_RGBA, false);
			if (texture) material.RegisterTexture("TexMetallic", texture);
		}

		if (aMaterial->GetTextureCount(aiTextureType_SHININESS) > 0)
		{
			aiString aPath;
			aMaterial->GetTexture(aiTextureType_SHININESS, 0, &aPath);
			std::string filename = processPath(&aPath, directory);

			Texture texture = TextureManager::LoadLocalTexture2D(filename, filename, GL_RGBA, false);
			if (texture) material.RegisterTexture("TexRoughness", texture);
		}

		if (aMaterial->GetTextureCount(aiTextureType_AMBIENT) > 0)
		{
			aiString aPath;
			aMaterial->GetTexture(aiTextureType_AMBIENT, 0, &aPath);
			std::string filename = processPath(&aPath, directory);

			Texture texture = TextureManager::LoadLocalTexture2D(filename, filename, GL_RGBA, false);
			if (texture) material.RegisterTexture("TexAO", texture);
		}

		return material;
	}
}