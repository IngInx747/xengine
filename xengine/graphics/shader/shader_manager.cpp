#include "shader_manager.h"

#include <string>
#include <fstream>

#include <utility/log.h>

namespace xengine
{
	std::vector<std::shared_ptr<Shader>> ShaderManager::_shaders{};
	std::unordered_map<std::string, Shader*> ShaderManager::_shaderTable{};

	void ShaderManager::Initialize()
	{
		generateDefaultShader();
	}

	void ShaderManager::Clear()
	{
		_shaders.clear();
		_shaderTable.clear();
	}
	
	Shader* ShaderManager::Get(const std::string& name)
	{
		auto it = _shaderTable.find(name);
		if (it != _shaderTable.end()) return it->second;

		Log::Message("[ShaderManager] Shader \"" + name + "\" not found", Log::WARN);
		return nullptr;
	}

	Shader* ShaderManager::LoadVertFragShader(
		const std::string& name,
		const std::string& vsPath,
		const std::string& fsPath,
		const std::vector<std::string>& defines)
	{
		auto it = _shaderTable.find(name);
		if (it != _shaderTable.end()) return it->second;

		Log::Message("[ShaderManager] Loading shader \"" + name + "\" from \"" + vsPath + "\" and \"" + fsPath + "\" ...", Log::INFO);

		std::shared_ptr<Shader> shader = loadVertFragShader(vsPath, fsPath, defines);

		if (!shader)
		{
			Log::Message("[ShaderManager] Shader \"" + name + "\" loading failed", Log::ERROR);
			return nullptr;
		}

		_shaders.push_back(shader);
		_shaderTable[name] = shader.get();

		Log::Message("[ShaderManager] Shader \"" + name + "\" loaded successfully", Log::INFO);

		return shader.get();
	}

	std::shared_ptr<Shader> ShaderManager::loadVertFragShader(
		const std::string& vsPath,
		const std::string& fsPath,
		const std::vector<std::string>& defines)
	{
		std::string vsSource = readsrc(vsPath);
		std::string fsSource = readsrc(fsPath);
		std::shared_ptr<Shader> shader = std::make_shared<Shader>();
		shader->GenerateVertFragShader(vsSource, fsSource, defines);
		return shader;
	}

	std::string ShaderManager::readsrc(const std::string& path)
	{
		std::string directory = path.substr(0, path.find_last_of("/\\"));
		std::string source{}, line;
		std::ifstream in(path, std::ios::in);

		if (!in)
		{
			Log::Message("[ShaderManager] Cannot open shader source \"" + path + "\"", Log::ERROR);
			return "";
		}

		while (std::getline(in, line))
		{
			// if we encounter an #include line, include other shader source
			if (line.substr(0, 8) == "#include")
			{
				std::string includePath = directory + "/" + line.substr(9);
				source.append(readsrc(includePath));
			}
			else
			{
				source.append(line + "\n");
			}
		}

		return source;
	}

	void ShaderManager::generateDefaultShader()
	{
		Shader* shader = nullptr;

		/// deferred

		shader = LoadVertFragShader("deferred ambient", "shaders/deferred/screen_ambient.vs", "shaders/deferred/ambient.fs");
		shader->Use();
		shader->SetUniform("gPositionMetallic", 0);
		shader->SetUniform("gNormalRoughness", 1);
		shader->SetUniform("gAlbedoAO", 2);
		shader->SetUniform("envIrradiance", 3);
		shader->SetUniform("envPrefilter", 4);
		shader->SetUniform("BRDFLUT", 5);
		shader->SetUniform("TexSSAO", 6);

		shader = LoadVertFragShader("deferred irradiance", "shaders/deferred/ambient_irradience.vs", "shaders/deferred/ambient_irradience.fs");
		shader->Use();
		shader->SetUniform("gPositionMetallic", 0);
		shader->SetUniform("gNormalRoughness", 1);
		shader->SetUniform("gAlbedoAO", 2);
		shader->SetUniform("envIrradiance", 3);
		shader->SetUniform("envPrefilter", 4);
		shader->SetUniform("BRDFLUT", 5);
		shader->SetUniform("TexSSAO", 6);

		shader = LoadVertFragShader("deferred directional", "shaders/deferred/screen_directional.vs", "shaders/deferred/directional.fs");
		//shader->Use();
		//shader->SetUniform("gPositionMetallic", 0);
		//shader->SetUniform("gNormalRoughness", 1);
		//shader->SetUniform("gAlbedoAO", 2);
		//shader->SetUniform("lightShadowMap", 3);

		shader = LoadVertFragShader("deferred point", "shaders/deferred/point.vs", "shaders/deferred/point.fs");
		//shader->Use();
		//shader->SetUniform("gPositionMetallic", 0);
		//shader->SetUniform("gNormalRoughness", 1);
		//shader->SetUniform("gAlbedoAO", 2);

		/// lighting

		shader = LoadVertFragShader("shadow directional", "shaders/shadow_cast.vs", "shaders/shadow_cast.fs");

		shader = LoadVertFragShader("volumn light", "shaders/light.vs", "shaders/light.fs");

		/// blit

		shader = LoadVertFragShader("blit", "shaders/screen_quad.vs", "shaders/default_blit.fs");
	}
}