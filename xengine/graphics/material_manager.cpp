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
	// lookup table
	std::unordered_map<std::string, Material> MaterialManager::g_localTable;
	std::unordered_map<std::string, Material> MaterialManager::g_globalTable;

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
		g_localTable.clear();
	}

	void MaterialManager::ClearGlobal()
	{
		g_globalTable.clear();
	}

	void MaterialManager::LoadLocalMaterial(const Material & other, const std::string & name)
	{
		g_localTable[name] = other;
	}

	void MaterialManager::LoadGlobalMaterial(const Material & other, const std::string & name)
	{
		g_globalTable[name] = other;
	}

	Material MaterialManager::Get(const std::string& name)
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

		Log::Message("[MaterialManager] Material \"" + name + "\" not found", Log::WARN);
		return Material();
	}

	void MaterialManager::generateDefaultMaterial()
	{
		// deferred (with TBN) (deferred pipeline OK)
		{
			std::string name{ "deferred" };

			Shader shader = ShaderManager::LoadGlobalVF(name, "shaders/deferred/g_buffer.vs", "shaders/deferred/g_buffer.fs", { "MESH_TBN" });

			Material material(shader);
			material.type = Material::DEFERRED;
			material.RegisterTexture("TexAlbedo", TextureManager::Get("chessboard"));
			material.RegisterTexture("TexNormal", TextureManager::Get("normal"));
			material.RegisterTexture("TexMetallic", TextureManager::Get("black"));
			material.RegisterTexture("TexRoughness", TextureManager::Get("chessboard"));
			material.RegisterTexture("TexAO", TextureManager::Get("white"));

			g_globalTable[name] = material;
		}

		// deferred (w/o TBN) (deferred pipeline OK)
		{
			std::string name{ "deferred no TBN" };

			Shader shader = ShaderManager::LoadGlobalVF(name, "shaders/deferred/g_buffer.vs", "shaders/deferred/g_buffer.fs");

			Material material(shader);
			material.type = Material::DEFERRED;
			material.RegisterTexture("TexAlbedo", TextureManager::Get("chessboard"));
			material.RegisterTexture("TexNormal", TextureManager::Get("normal"));
			material.RegisterTexture("TexMetallic", TextureManager::Get("black")); // default: no metallic
			material.RegisterTexture("TexRoughness", TextureManager::Get("chessboard"));
			material.RegisterTexture("TexAO", TextureManager::Get("white")); // default: no AO

			g_globalTable[name] = material;
		}

		// glass (deferred pipeline NOT OK due to transparency sorting)
		//{
		//	std::string name{ "glass" };
		//
		//	Shader* shader = ShaderManager::LoadGlobalVF(name, "shaders/forward_render.vs", "shaders/forward_render.fs", { "ALPHA_BLEND" });
		//
		//	std::shared_ptr<Material> material = loadMaterial(shader);
		//	material->type = Material::FORWARD;
		//	material->RegisterTexture("TexAlbedo", TextureManager::LoadLocalTexture2D("glass albedo", "textures/glass.png", GL_RGBA));
		//	material->RegisterTexture("TexNormal", TextureManager::LoadLocalTexture2D("glass normal", "textures/pbr/plastic/normal.png", GL_RGBA));
		//	material->RegisterTexture("TexMetallic", TextureManager::LoadLocalTexture2D("glass metallic", "textures/pbr/plastic/metallic.png", GL_RGBA));
		//	material->RegisterTexture("TexRoughness", TextureManager::LoadLocalTexture2D("glass roughness", "textures/pbr/plastic/roughness.png", GL_RGBA));
		//	material->RegisterTexture("TexAO", TextureManager::LoadLocalTexture2D("glass ao", "textures/pbr/plastic/ao.png", GL_RGBA));
		//	material->attribute.bBlend = true;
		//
		//	_defaultMaterials.push_back(material);
		//	_defaultMaterialTable[name] = material.get();
		//}

		// alpha blend
		{
			std::string name{ "alpha blend" };

			Shader shader = ShaderManager::LoadGlobalVF(name, "shaders/forward_render.vs", "shaders/forward_render.fs", { "ALPHA_BLEND" });

			Material material(shader);
			material.type = Material::FORWARD;
			material.attribute.bBlend = true;

			g_globalTable[name] = material;
		}

		// alpha cutout
		{
			std::string name{ "alpha discard" };

			Shader shader = ShaderManager::LoadGlobalVF(name, "shaders/forward_render.vs", "shaders/forward_render.fs", { "ALPHA_DISCARD" });

			Material material(shader);
			material.type = Material::FORWARD;
			material.attribute.bBlend = false;

			g_globalTable[name] = material;
		}

		// sky box
		{
			std::string name{ "skybox" };

			Shader shader = ShaderManager::LoadGlobalVF("background", "shaders/background.vs", "shaders/background.fs");

			Material material(shader);
			material.RegisterUniform("Exposure", 1.0f);
			material.attribute.eDepthFunc = GL_LEQUAL;
			material.attribute.bCull = false;
			material.attribute.bShadowCast = false;
			material.attribute.bShadowRecv = false;

			g_globalTable[name] = material;
		}

		// normal vector debug
		{
			std::string name{ "normal debug" };
			Shader shader = ShaderManager::LoadGlobalVF(name, "shaders/debug_forward.vs", "shaders/debug_forward.fs");

			Material material(shader);
			material.type = Material::FORWARD;
			material.attribute.bBlend = false;

			g_globalTable[name] = material;
		}
	}
}