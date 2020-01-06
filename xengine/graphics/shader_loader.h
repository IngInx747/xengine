#pragma once
#ifndef XE_SHADER_LOADER_H
#define XE_SHADER_LOADER_H

#include <string>
#include <vector>

#include "shader.h"

namespace xengine
{
	// create a shader of TYPE
	unsigned int CreateShader(const std::string& source, unsigned int type);

	// compile a vertex shader
	unsigned int CreateVertexShader(const std::string& source, unsigned int type);

	// compile a geometry shader
	unsigned int CreateGeometryShader(const std::string& source, unsigned int type);

	// compile a fragment shader
	unsigned int CreateFragmentShader(const std::string& source, unsigned int type);

	// read shader source from file (if include exists, read included files recursively)
	std::string ReadShaderSource(const std::string& path);

	// insert precompile flag to shader source
	std::string InsertShaderDefine(const std::string& source, const std::vector<std::string>& defines);

	// load a vertex-fragment shader program
	Shader LoadShaderVF(
		const std::string& vsPath,
		const std::string& fsPath,
		const std::vector<std::string>& defines);

	// load a vertex-geometry-fragment shader program
	Shader LoadShaderVGF(
		const std::string& vsPath,
		const std::string& gsPath,
		const std::string& fsPath,
		const std::vector<std::string>& defines);
}

#endif // !XE_SHADER_LOADER_H
