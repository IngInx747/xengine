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
		// initialize shader manager (load default resources)
		static void Initialize();

		// clear resources
		static void Clear();

		// clear local resources (loaded with scene)
		static void ClearLocal();

		// clear global resources (default resources or shared by multiple scenes)
		static void ClearGlobal();

		// get a named shader
		static Shader Get(const std::string& name);

		// load a vertex-fragment shader program as local resource
		static Shader LoadLocalVF(
			const std::string& name,
			const std::string& vsPath,
			const std::string& fsPath,
			const std::vector<std::string>& defines = {});

		// load a vertex-geometry-fragment shader program as local resource
		static Shader LoadLocalVGF(
			const std::string& name,
			const std::string& vsPath,
			const std::string& gsPath,
			const std::string& fsPath,
			const std::vector<std::string>& defines = {});

		// load a vertex-fragment shader program as global resource
		static Shader LoadGlobalVF(
			const std::string& name,
			const std::string& vsPath,
			const std::string& fsPath,
			const std::vector<std::string>& defines = {});

		// load a vertex-geometry-fragment shader program as global resource
		static Shader LoadGlobalVGF(
			const std::string& name,
			const std::string& vsPath,
			const std::string& gsPath,
			const std::string& fsPath,
			const std::vector<std::string>& defines = {});

	private:
		static Shader loadVF(
			std::unordered_map<std::string, Shader>& table,
			const std::string& name,
			const std::string& vsPath,
			const std::string& fsPath,
			const std::vector<std::string>& defines = {});

		static Shader loadVGF(
			std::unordered_map<std::string, Shader>& table,
			const std::string& name,
			const std::string& vsPath,
			const std::string& gsPath,
			const std::string& fsPath,
			const std::vector<std::string>& defines = {});

	private:
		// local lookup tables
		static std::unordered_map<std::string, Shader> g_localTable;

	private:
		// global lookup tables
		static std::unordered_map<std::string, Shader> g_globalTable;
	};
}

#endif // !XE_SHADER_MANAGER_H
