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

		// load a vertex-fragment shader program
		static Shader* LoadVF(
			const std::string& name,
			const std::string& vsPath,
			const std::string& fsPath,
			const std::vector<std::string>& defines = {});

		// load a vertex-geometry-fragment shader program
		static Shader* LoadVGF(
			const std::string& name,
			const std::string& vsPath,
			const std::string& gsPath,
			const std::string& fsPath,
			const std::vector<std::string>& defines = {});

	private:
		static std::shared_ptr<Shader> loadVF(
			const std::string& vsPath,
			const std::string& fsPath,
			const std::vector<std::string>& defines);

		static std::shared_ptr<Shader> loadVGF(
			const std::string& vsPath,
			const std::string& gsPath,
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
