#include "material_manager.h"

#include <vendor/glad/glad.h>

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

	void MaterialManager::RegisterGlobalMaterial(const std::string & name, const Material & other)
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

			RegisterGlobalMaterial(name, material);
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

			RegisterGlobalMaterial(name, material);
		}

		// alpha blend
		{
			std::string name{ "alpha blend" };

			Shader shader = ShaderManager::LoadGlobalVF(name, "shaders/forward_render.vs", "shaders/forward_render.fs", { "ALPHA_BLEND" });

			Material material(shader);
			material.type = Material::FORWARD;
			material.attribute.bBlend = true;

			RegisterGlobalMaterial(name, material);
		}

		// alpha cutout
		{
			std::string name{ "alpha discard" };

			Shader shader = ShaderManager::LoadGlobalVF(name, "shaders/forward_render.vs", "shaders/forward_render.fs", { "ALPHA_DISCARD" });

			Material material(shader);
			material.type = Material::FORWARD;
			material.attribute.bBlend = false;

			RegisterGlobalMaterial(name, material);
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

			RegisterGlobalMaterial(name, material);
		}

		// normal vector debug
		{
			std::string name{ "normal debug" };
			Shader shader = ShaderManager::LoadGlobalVF(name, "shaders/debug_forward.vs", "shaders/debug_forward.fs");

			Material material(shader);
			material.type = Material::FORWARD;
			material.attribute.bBlend = false;

			RegisterGlobalMaterial(name, material);
		}
	}
}