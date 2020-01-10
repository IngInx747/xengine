#include "shader_manager.h"

#include <utility/log.h>

#include "shader_loader.h"

namespace xengine
{
	std::unordered_map<std::string, Shader> ShaderManager::g_localTable{};
	std::unordered_map<std::string, Shader> ShaderManager::g_globalTable{};

	void ShaderManager::Initialize()
	{
	}

	void ShaderManager::Clear()
	{
		ClearLocal();
		ClearGlobal();
	}

	void ShaderManager::ClearLocal()
	{
		g_localTable.clear();
	}

	void ShaderManager::ClearGlobal()
	{
		g_globalTable.clear();
	}
	
	Shader ShaderManager::Get(const std::string& name)
	{
		{
			auto it = g_localTable.find(name);
			if (it != g_localTable.end()) return it->second;
		}

		{
			auto it = g_globalTable.find(name);
			if (it != g_globalTable.end()) return it->second;
		}

		Log::Message("[ShaderManager] Shader \"" + name + "\" not found", Log::WARN);
		return Shader();
	}

	Shader ShaderManager::LoadLocalVF(const std::string & name, const std::string & vsPath, const std::string & fsPath, const std::vector<std::string>& defines)
	{
		return loadVF(g_localTable, name, vsPath, fsPath, defines);
	}

	Shader ShaderManager::LoadLocalVGF(const std::string & name, const std::string & vsPath, const std::string & gsPath, const std::string & fsPath, const std::vector<std::string>& defines)
	{
		return loadVGF(g_localTable, name, vsPath, gsPath, fsPath, defines);
	}

	Shader ShaderManager::LoadGlobalVF(const std::string& name, const std::string& vsPath, const std::string& fsPath, const std::vector<std::string>& defines)
	{
		return loadVF(g_globalTable, name, vsPath, fsPath, defines);
	}

	Shader ShaderManager::LoadGlobalVGF(const std::string & name, const std::string & vsPath, const std::string & gsPath, const std::string & fsPath, const std::vector<std::string>& defines)
	{
		return loadVGF(g_globalTable, name, vsPath, gsPath, fsPath, defines);
	}

	Shader ShaderManager::loadVF(
		std::unordered_map<std::string, Shader>& table,
		const std::string & name,
		const std::string & vsPath,
		const std::string & fsPath,
		const std::vector<std::string>& defines)
	{
		auto it = table.find(name);
		if (it != table.end()) return it->second;

		Log::Message("[ShaderManager] Loading shader \"" + name + "\" from \"" + vsPath + "\", \"" + fsPath + "\" ...", Log::INFO);

		Shader shader = LoadShaderVF(vsPath, fsPath, defines);

		if (!shader)
		{
			Log::Message("[ShaderManager] Shader \"" + name + "\" loading failed", Log::ERROR);
			return Shader();
		}

		table[name] = shader;

		Log::Message("[ShaderManager] Shader \"" + name + "\" loaded successfully", Log::INFO);

		return shader;
	}

	Shader ShaderManager::loadVGF(
		std::unordered_map<std::string, Shader>& table,
		const std::string & name,
		const std::string & vsPath,
		const std::string & gsPath,
		const std::string & fsPath,
		const std::vector<std::string>& defines)
	{
		auto it = table.find(name);
		if (it != table.end()) return it->second;

		Log::Message("[ShaderManager] Loading shader \"" + name + "\" from \"" + vsPath + "\", \"" + gsPath + "\", \"" + fsPath + "\" ...", Log::INFO);

		Shader shader = LoadShaderVGF(vsPath, gsPath, fsPath, defines);

		if (!shader)
		{
			Log::Message("[ShaderManager] Shader \"" + name + "\" loading failed", Log::ERROR);
			return Shader();
		}

		table[name] = shader;

		Log::Message("[ShaderManager] Shader \"" + name + "\" loaded successfully", Log::INFO);

		return shader;
	}
}