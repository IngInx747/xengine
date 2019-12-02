#pragma once
#ifndef XE_SHADER_MANAGER_H
#define XE_SHADER_MANAGER_H

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

#include "shader.h"

namespace xengine
{
	class ShaderManager
	{
	public:
		static void Initialize();
		static void Clear();

		// get a named shader
		static Shader* Get(const std::string& name);

		static Shader* LoadVertFragShader(
			const std::string& name,
			const std::string& vsPath,
			const std::string& fsPath,
			const std::vector<std::string>& defines = {});

	private:
		// read shader source from file
		static std::string readsrc(const std::string& path);

		static std::shared_ptr<Shader> loadVertFragShader(
			const std::string& vsPath,
			const std::string& fsPath,
			const std::vector<std::string>& defines);

		static void generateDefaultShader();

	private:
		// resources containers
		static std::vector<std::shared_ptr<Shader>> _shaders;

		// lookup tables
		static std::unordered_map<std::string, Shader*> _shaderTable;
	};
}

#endif // !XE_SHADER_MANAGER_H
