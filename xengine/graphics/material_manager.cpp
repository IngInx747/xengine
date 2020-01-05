#include "material_manager.h"

#include <vendor/glad/glad.h>

#include <vendor/assimp/scene.h>
#include <vendor/assimp/Importer.hpp>
#include <vendor/assimp/postprocess.h>

#include <utility/log.h>

#include "shader_manager.h"
#include "texture_manager.h"

namespace xengine
{
	std::vector<std::shared_ptr<Material>> MaterialManager::_materials{};
	std::unordered_map<std::string, Material*> MaterialManager::_materialTable{};

	std::vector<std::shared_ptr<Material>> MaterialManager::_defaultMaterials{};
	std::unordered_map<std::string, Material*> MaterialManager::_defaultMaterialTable{};

	void MaterialManager::Initialize()
	{
		generateDefaultMaterial();
	}

	void MaterialManager::Clear()
	{
		ClearLocal();
		ClearGlobal();
	}

	void MaterialManager::ClearLocal()
	{
		_materialTable.clear();
		_materials.clear();
	}

	void MaterialManager::ClearGlobal()
	{
		_defaultMaterialTable.clear();
		_defaultMaterials.clear();
	}

	Material* MaterialManager::Load(const Shader& shader)
	{
		std::shared_ptr<Material> material = loadMaterial(shader);

		if (!material)
		{
			Log::Message("[MaterialManager] Material \"" + std::to_string(shader.ID()) + "\" loading failed", Log::ERROR);
			return nullptr;
		}

		Log::Message("[MaterialManager] Material \"" + std::to_string(shader.ID()) + "\" loaded successfully", Log::INFO);
		_materials.push_back(material);

		return material.get();
	}

	Material* MaterialManager::Load(const std::string& name, const Shader& shader)
	{
		// search for scene-specific resources
		auto it = _materialTable.find(name);
		if (it != _materialTable.end()) return it->second;

		// Note: Here we do not search in default resources, so the material can override
		// the default material, which possibly already exists and has the same name.

		std::shared_ptr<Material> material = loadMaterial(shader);

		if (!material)
		{
			Log::Message("[MaterialManager] Material \"" + name + "\" loading failed", Log::ERROR);
			return nullptr;
		}

		Log::Message("[MaterialManager] Material \"" + name + "\" loaded successfully", Log::INFO);
		_materials.push_back(material);
		_materialTable[name] = material.get();

		return material.get();
	}

	Material* MaterialManager::Get(const std::string& name)
	{
		// search for scene-specific resources
		{
			auto it = _materialTable.find(name);
			if (it != _materialTable.end()) return CopyFromOther(it->second);
		}

		// if texture was not loaded along with the scene, search for default resources
		{
			auto it = _defaultMaterialTable.find(name);
			if (it != _defaultMaterialTable.end()) return CopyFromOther(it->second);
		}

		Log::Message("[MaterialManager] Material \"" + name + "\" not found", Log::WARN);
		return nullptr;
	}

	Material* MaterialManager::CopyFromOther(Material* other)
	{
		// deepcopy
		std::shared_ptr<Material> copy = std::make_shared<Material>(*other);
		
		// record new copy
		_materials.push_back(copy);

		return copy.get();
	}

	Material* MaterialManager::CopyFromOther(const std::string& name)
	{
		Material* material = Get(name);

		if (!material)
		{
			Log::Message("[MaterialManager] Copying material \"" + name + "\" failed", Log::WARN);
			return nullptr;
		}

		return CopyFromOther(material);
	}

	std::shared_ptr<Material> MaterialManager::loadMaterial(const Shader& shader)
	{
		std::shared_ptr<Material> material = std::make_shared<Material>(shader);
		return material;
	}

	void MaterialManager::generateDefaultMaterial()
	{
		// deferred (with TBN) (deferred pipeline OK)
		{
			std::string name{ "deferred" };

			Shader shader = ShaderManager::LoadGlobalVF(name, "shaders/deferred/g_buffer.vs", "shaders/deferred/g_buffer.fs", { "MESH_TBN" });

			std::shared_ptr<Material> material = loadMaterial(shader);
			material->type = Material::DEFERRED;
			material->RegisterTexture("TexAlbedo", TextureManager::Get("chessboard"));
			material->RegisterTexture("TexNormal", TextureManager::Get("normal"));
			material->RegisterTexture("TexMetallic", TextureManager::Get("black"));
			material->RegisterTexture("TexRoughness", TextureManager::Get("chessboard"));
			material->RegisterTexture("TexAO", TextureManager::Get("white"));

			_defaultMaterials.push_back(material);
			_defaultMaterialTable[name] = material.get();
		}

		// deferred (w/o TBN) (deferred pipeline OK)
		{
			std::string name{ "deferred no TBN" };

			Shader shader = ShaderManager::LoadGlobalVF(name, "shaders/deferred/g_buffer.vs", "shaders/deferred/g_buffer.fs");

			std::shared_ptr<Material> material = loadMaterial(shader);
			material->type = Material::DEFERRED;
			material->RegisterTexture("TexAlbedo", TextureManager::Get("chessboard"));
			material->RegisterTexture("TexNormal", TextureManager::Get("normal"));
			material->RegisterTexture("TexMetallic", TextureManager::Get("black")); // default: no metallic
			material->RegisterTexture("TexRoughness", TextureManager::Get("chessboard"));
			material->RegisterTexture("TexAO", TextureManager::Get("white")); // default: no AO

			_defaultMaterials.push_back(material);
			_defaultMaterialTable[name] = material.get();
		}

		// glass (deferred pipeline NOT OK due to transparency sorting)
		//{
		//	std::string name{ "glass" };
		//
		//	Shader* shader = ShaderManager::LoadGlobalVF(name, "shaders/forward_render.vs", "shaders/forward_render.fs", { "ALPHA_BLEND" });
		//
		//	std::shared_ptr<Material> material = loadMaterial(shader);
		//	material->type = Material::FORWARD;
		//	material->RegisterTexture("TexAlbedo", TextureManager::LoadTexture2D("glass albedo", "textures/glass.png", GL_RGBA));
		//	material->RegisterTexture("TexNormal", TextureManager::LoadTexture2D("glass normal", "textures/pbr/plastic/normal.png", GL_RGBA));
		//	material->RegisterTexture("TexMetallic", TextureManager::LoadTexture2D("glass metallic", "textures/pbr/plastic/metallic.png", GL_RGBA));
		//	material->RegisterTexture("TexRoughness", TextureManager::LoadTexture2D("glass roughness", "textures/pbr/plastic/roughness.png", GL_RGBA));
		//	material->RegisterTexture("TexAO", TextureManager::LoadTexture2D("glass ao", "textures/pbr/plastic/ao.png", GL_RGBA));
		//	material->attribute.bBlend = true;
		//
		//	_defaultMaterials.push_back(material);
		//	_defaultMaterialTable[name] = material.get();
		//}

		// alpha blend
		{
			std::string name{ "alpha blend" };

			Shader shader = ShaderManager::LoadGlobalVF(name, "shaders/forward_render.vs", "shaders/forward_render.fs", { "ALPHA_BLEND" });

			std::shared_ptr<Material> material = loadMaterial(shader);
			material->type = Material::FORWARD;
			material->attribute.bBlend = true;

			_defaultMaterials.push_back(material);
			_defaultMaterialTable[name] = material.get();
		}

		// alpha cutout
		{
			std::string name{ "alpha discard" };

			Shader shader = ShaderManager::LoadGlobalVF(name, "shaders/forward_render.vs", "shaders/forward_render.fs", { "ALPHA_DISCARD" });

			std::shared_ptr<Material> material = loadMaterial(shader);
			material->type = Material::FORWARD;
			material->attribute.bBlend = false;

			_defaultMaterials.push_back(material);
			_defaultMaterialTable[name] = material.get();
		}

		// sky box
		{
			std::string name{ "skybox" };

			Shader shader = ShaderManager::LoadGlobalVF("background", "shaders/background.vs", "shaders/background.fs");

			std::shared_ptr<Material> material = loadMaterial(shader);
			material->RegisterUniform("Exposure", 1.0f);
			material->attribute.eDepthFunc = GL_LEQUAL;
			material->attribute.bCull = false;
			material->attribute.bShadowCast = false;
			material->attribute.bShadowRecv = false;

			_defaultMaterials.push_back(material);
			_defaultMaterialTable[name] = material.get();
		}

		// normal vector debug
		{
			std::string name{ "normal debug" };
			Shader shader = ShaderManager::LoadGlobalVF(name, "shaders/debug_forward.vs", "shaders/debug_forward.fs");

			std::shared_ptr<Material> material = loadMaterial(shader);
			material->type = Material::FORWARD;
			material->attribute.bBlend = false;

			_defaultMaterials.push_back(material);
			_defaultMaterialTable[name] = material.get();
		}
	}

	////////////////////////////////////////////////////////////////
	// API
	////////////////////////////////////////////////////////////////

	std::string processPath(aiString* aPath, const std::string& directory)
	{
		std::string path = std::string(aPath->C_Str());

		// parse path directly if path contains "/" indicating it is an absolute path
		// otherwise parse as relative.
		if (path.find(":/") == std::string::npos || path.find(":\\") == std::string::npos)
			path = directory + "/" + path;

		return path;
	}

	Material * MaterialManager::LoadFromModel(aiMaterial * aMaterial, const std::string & directory, aiMesh* aMesh)
	{
		Material* material = nullptr;
		aiString aString;
		aMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &aString);
		std::string texDiffPath{ aString.C_Str() };
		bool alpha = false;

		if (texDiffPath.find("_alpha") != std::string::npos)
		{
			material = CopyFromOther("alpha discard");
			alpha = true;
		}
		else
		{
			if (aMesh->HasTangentsAndBitangents())
				material = CopyFromOther("deferred");
			else
				material = CopyFromOther("deferred no TBN");
		}

		if (aMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			aiString aPath;
			aMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &aPath);
			std::string filename = processPath(&aPath, directory);

			Texture texture = TextureManager::LoadTexture2D(filename, filename, alpha ? GL_RGBA : GL_RGB, true);
			if (texture) material->RegisterTexture("TexAlbedo", texture);
		}

		if (aMaterial->GetTextureCount(aiTextureType_DISPLACEMENT) > 0)
		{
			aiString aPath;
			aMaterial->GetTexture(aiTextureType_DISPLACEMENT, 0, &aPath);
			std::string filename = processPath(&aPath, directory);

			Texture texture = TextureManager::LoadTexture2D(filename, filename, GL_RGBA, false);
			if (texture) material->RegisterTexture("TexNormal", texture);
		}

		if (aMaterial->GetTextureCount(aiTextureType_SPECULAR) > 0)
		{
			aiString aPath;
			aMaterial->GetTexture(aiTextureType_SPECULAR, 0, &aPath);
			std::string filename = processPath(&aPath, directory);

			Texture texture = TextureManager::LoadTexture2D(filename, filename, GL_RGBA, false);
			if (texture) material->RegisterTexture("TexMetallic", texture);
		}

		if (aMaterial->GetTextureCount(aiTextureType_SHININESS) > 0)
		{
			aiString aPath;
			aMaterial->GetTexture(aiTextureType_SHININESS, 0, &aPath);
			std::string filename = processPath(&aPath, directory);

			Texture texture = TextureManager::LoadTexture2D(filename, filename, GL_RGBA, false);
			if (texture) material->RegisterTexture("TexRoughness", texture);
		}

		if (aMaterial->GetTextureCount(aiTextureType_AMBIENT) > 0)
		{
			aiString aPath;
			aMaterial->GetTexture(aiTextureType_AMBIENT, 0, &aPath);
			std::string filename = processPath(&aPath, directory);

			Texture texture = TextureManager::LoadTexture2D(filename, filename, GL_RGBA, false);
			if (texture) material->RegisterTexture("TexAO", texture);
		}

		return material;
	}
}