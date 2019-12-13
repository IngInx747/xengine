#include "shader_manager.h"

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

	Shader* ShaderManager::LoadVF(
		const std::string& name,
		const std::string& vsPath,
		const std::string& fsPath,
		const std::vector<std::string>& defines)
	{
		auto it = _shaderTable.find(name);
		if (it != _shaderTable.end()) return it->second;

		Log::Message("[ShaderManager] Loading shader \"" + name + "\" from \"" + vsPath + "\", \"" + fsPath + "\" ...", Log::INFO);

		std::shared_ptr<Shader> shader = loadVF(vsPath, fsPath, defines);

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

	Shader * ShaderManager::LoadVGF(
		const std::string & name,
		const std::string & vsPath,
		const std::string & gsPath,
		const std::string & fsPath,
		const std::vector<std::string>& defines)
	{
		auto it = _shaderTable.find(name);
		if (it != _shaderTable.end()) return it->second;

		Log::Message("[ShaderManager] Loading shader \"" + name + "\" from \"" + vsPath + "\", \"" + gsPath + "\", \"" + fsPath + "\" ...", Log::INFO);

		std::shared_ptr<Shader> shader = loadVGF(vsPath, gsPath, fsPath, defines);

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

	std::shared_ptr<Shader> ShaderManager::loadVF(
		const std::string& vsPath,
		const std::string& fsPath,
		const std::vector<std::string>& defines)
	{
		std::shared_ptr<Shader> shader = std::make_shared<Shader>();

		shader->AttachVertexShader(InsertShaderDefine(ReadShaderSource(vsPath), defines));
		shader->AttachFragmentShader(InsertShaderDefine(ReadShaderSource(fsPath), defines));

		shader->GenerateAndLink();

		return shader;
	}

	std::shared_ptr<Shader> ShaderManager::loadVGF(
		const std::string & vsPath,
		const std::string & gsPath,
		const std::string & fsPath,
		const std::vector<std::string>& defines)
	{
		std::shared_ptr<Shader> shader = std::make_shared<Shader>();

		shader->AttachVertexShader(InsertShaderDefine(ReadShaderSource(vsPath), defines));
		shader->AttachGeometryShader(InsertShaderDefine(ReadShaderSource(gsPath), defines));
		shader->AttachFragmentShader(InsertShaderDefine(ReadShaderSource(fsPath), defines));

		shader->GenerateAndLink();

		return shader;
	}

	void ShaderManager::generateDefaultShader()
	{
	}
}