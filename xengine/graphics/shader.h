#pragma once
#ifndef XE_SHADER_H
#define XE_SHADER_H

#include <string>
#include <vector>
#include <unordered_map>

#include <vendor/glm/common.hpp>

#include <utility/smart_handle.h>

namespace xengine
{
	unsigned int CreateShader(const std::string& source, unsigned int type); // create a shader of TYPE
	unsigned int CreateVertexShader(const std::string& source, unsigned int type);   // compile a vertex shader
	unsigned int CreateGeometryShader(const std::string& source, unsigned int type); // compile a geometry shader
	unsigned int CreateFragmentShader(const std::string& source, unsigned int type); // compile a fragment shader

	// read shader source from file (if include exists, read included files recursively)
	std::string ReadShaderSource(const std::string& path);

	// insert precompile flag to shader source
	std::string InsertShaderDefine(const std::string& source, const std::vector<std::string>& defines);

	class ShaderMomory : public SharedMemory
	{
	public:
		struct VarTableEntry
		{
			unsigned int type;
			unsigned int size;
			unsigned int location;
		};

	public:
		void Generate(); // allocate memory on GPU
		void Destory(); // free memory on GPU
		
	public:
		unsigned int m_id = 0; // shader program id

		unsigned int m_vs = 0; // vertex shader id
		unsigned int m_gs = 0; // geometry shader id
		unsigned int m_fs = 0; // fragment shader id
		unsigned int m_ts = 0; // tessellation shader id
		unsigned int m_cs = 0; // compute shader id

		std::unordered_map<std::string, VarTableEntry> m_attributeTable;
		std::unordered_map<std::string, VarTableEntry> m_uniformTable;
	};

	class Shader : public SharedHandle<ShaderMomory>
	{
	public:
		struct VarTableEntry
		{
			unsigned int type;
			unsigned int size;
			unsigned int location;
		};

	public:
		// use the shader program (make this shader as current active)
		void Bind();
		void Bind() const;
		
		// stop using the shader program
		void Unbind() const;

		// return location of uniform variable, -1 if not exist
		int GetUniformLocation(const std::string& name);

		// set value to uniform variable
		void SetUniform(const std::string& location, int   value);
		void SetUniform(const std::string& location, bool  value);
		void SetUniform(const std::string& location, float value);
		void SetUniform(const std::string& location, unsigned int value);
		void SetUniform(const std::string& location, const glm::vec2& value);
		void SetUniform(const std::string& location, const glm::vec3& value);
		void SetUniform(const std::string& location, const glm::vec4& value);
		void SetUniform(const std::string& location, const glm::mat2& value);
		void SetUniform(const std::string& location, const glm::mat3& value);
		void SetUniform(const std::string& location, const glm::mat4& value);
		void SetUniform(const std::string& location, int size, const std::vector<glm::vec2>& values);
		void SetUniform(const std::string& location, int size, const std::vector<glm::vec3>& values);
		void SetUniform(const std::string& location, int size, const std::vector<glm::vec4>& values);

		// generate program alone w/o linking attached shaders
		void Generate();

		// compile shader source and attach shader to the program
		void AttachVertexShader(const std::string& source);   // compile a vertex shader and attach to the program
		void AttachGeometryShader(const std::string& source); // compile a geometry shader and attach to the program
		void AttachFragmentShader(const std::string& source); // compile a fragment shader and attach to the program

		// link all attached shaders
		void Link();

		// generate shader program and link all attached shaders
		void GenerateAndLink();

		// query active attributes and store result to lookup hash table
		void QueryActiveAttributes();

		// query active uniforms and store result to lookup hash table
		void QueryActiveUniforms();

		explicit operator bool() const { return m_ptr && m_ptr->m_id; }

		inline unsigned int ID() const { return m_ptr->m_id; }
	};
}

#endif // !XE_SHADER_H
