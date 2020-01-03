#include "shader.h"

#include <string>
#include <fstream>

#include <vendor/glad/glad.h>
#include <vendor/glm/glm.hpp>

#include <utility/log.h>

namespace xengine
{
	////////////////////////////////////////////////////////////////
	// Shader
	////////////////////////////////////////////////////////////////

	Shader::Shader()
	{
	}

	Shader::~Shader()
	{
		DeleteGpuData();
	}

	void Shader::Bind()
	{
		glUseProgram(m_id);
	}

	void Shader::Unbind()
	{
		glUseProgram(0);
	}

	void Shader::DeleteGpuData()
	{
		if (m_id) { glDeleteProgram(m_id); m_id = 0; }
	}

	int Shader::GetUniformLocation(const std::string& name)
	{
		auto it = m_uniformTable.find(name);
		if (it != m_uniformTable.end()) return it->second.location;
		return -1;
	}

	void Shader::SetUniform(const std::string& name, int value)
	{
		int loc = GetUniformLocation(name);
		if (loc >= 0) glUniform1i(loc, value);
	}

	void Shader::SetUniform(const std::string& name, bool value)
	{
		int loc = GetUniformLocation(name);
		if (loc >= 0) glUniform1i(loc, static_cast<int>(value));
	}

	void Shader::SetUniform(const std::string& name, float value)
	{
		int loc = GetUniformLocation(name);
		if (loc >= 0) glUniform1f(loc, value);
	}

	void Shader::SetUniform(const std::string& name, unsigned int value)
	{
		int loc = GetUniformLocation(name);
		if (loc >= 0) glUniform1i(loc, static_cast<int>(value));
	}

	void Shader::SetUniform(const std::string& name, const glm::vec2& value)
	{
		int loc = GetUniformLocation(name);
		if (loc >= 0) glUniform2fv(loc, 1, &value[0]);
	}

	void Shader::SetUniform(const std::string& name, const glm::vec3& value)
	{
		int loc = GetUniformLocation(name);
		if (loc >= 0) glUniform3fv(loc, 1, &value[0]);
	}

	void Shader::SetUniform(const std::string& name, const glm::vec4& value)
	{
		int loc = GetUniformLocation(name);
		if (loc >= 0) glUniform4fv(loc, 1, &value[0]);
	}

	void Shader::SetUniform(const std::string& name, int size, const std::vector<glm::vec2>& values)
	{
		int loc = glGetUniformLocation(m_id, name.c_str());
		if (loc >= 0) glUniform2fv(loc, size, (float*)(&values[0].x));
	}

	void Shader::SetUniform(const std::string& name, int size, const std::vector<glm::vec3>& values)
	{
		int loc = glGetUniformLocation(m_id, name.c_str());
		if (loc >= 0) glUniform3fv(loc, size, (float*)(&values[0].x));
	}

	void Shader::SetUniform(const std::string& name, int size, const std::vector<glm::vec4>& values)
	{
		int loc = glGetUniformLocation(m_id, name.c_str());
		if (loc >= 0) glUniform4fv(loc, size, (float*)(&values[0].x));
	}

	void Shader::SetUniform(const std::string& name, const glm::mat2& value)
	{
		int loc = GetUniformLocation(name);
		if (loc >= 0) glUniformMatrix2fv(loc, 1, GL_FALSE, &value[0][0]);
	}

	void Shader::SetUniform(const std::string& name, const glm::mat3& value)
	{
		int loc = GetUniformLocation(name);
		if (loc >= 0) glUniformMatrix3fv(loc, 1, GL_FALSE, &value[0][0]);
	}

	void Shader::SetUniform(const std::string& name, const glm::mat4& value)
	{
		int loc = GetUniformLocation(name);
		if (loc >= 0) glUniformMatrix4fv(loc, 1, GL_FALSE, &value[0][0]);
	}

	void Shader::Generate()
	{
		if (!m_id) m_id = glCreateProgram();
	}

	void Shader::AttachVertexShader(const std::string & source)
	{
		m_vs = CreateShader(source, GL_VERTEX_SHADER);
	}

	void Shader::AttachGeometryShader(const std::string & source)
	{
		m_gs = CreateShader(source, GL_GEOMETRY_SHADER);
	}

	void Shader::AttachFragmentShader(const std::string & source)
	{
		m_fs = CreateShader(source, GL_FRAGMENT_SHADER);
	}

	void Shader::Link()
	{
		if (m_id == 0)
		{
			Log::Message("[Shader] Shader program has not been created. \n", Log::ERROR);
			return;
		}

		int status;
		char log[1024];

		if (m_vs) glAttachShader(m_id, m_vs);
		if (m_gs) glAttachShader(m_id, m_gs);
		if (m_fs) glAttachShader(m_id, m_fs);

		glLinkProgram(m_id);

		if (m_vs) { glDetachShader(m_id, m_vs); glDeleteShader(m_vs); m_vs = 0; }
		if (m_gs) { glDetachShader(m_id, m_gs); glDeleteShader(m_gs); m_gs = 0; }
		if (m_fs) { glDetachShader(m_id, m_fs); glDeleteShader(m_fs); m_fs = 0; }

		glGetProgramiv(m_id, GL_LINK_STATUS, &status);
		if (!status)
		{
			glGetProgramInfoLog(m_id, 1024, NULL, log);
			Log::Message("[Shader] Program linking error: \n" + std::string(log), Log::ERROR);
			glDeleteProgram(m_id);
			m_id = 0;
		}

		QueryActiveAttributes();
		QueryActiveUniforms();
	}

	void Shader::GenerateAndLink()
	{
		Generate(); // in case program was not create
		Link();
	}

	void Shader::QueryActiveAttributes()
	{
		char buffer[128];
		int niv;

		glGetProgramiv(m_id, GL_ACTIVE_ATTRIBUTES, &niv);

		for (int i = 0; i < niv; ++i)
		{
			GLenum glType;
			GLsizei glSize;
			glGetActiveAttrib(m_id, i, sizeof(buffer), 0, &glSize, &glType, buffer);
			m_attributeTable[std::string(buffer)].type = glType;
			m_attributeTable[std::string(buffer)].size = glSize;
			m_attributeTable[std::string(buffer)].location = glGetAttribLocation(m_id, buffer);
		}
	}

	void Shader::QueryActiveUniforms()
	{
		char buffer[128];
		int niv;

		glGetProgramiv(m_id, GL_ACTIVE_UNIFORMS, &niv);

		for (int i = 0; i < niv; ++i)
		{
			GLenum glType;
			GLsizei glSize;
			glGetActiveUniform(m_id, i, sizeof(buffer), 0, &glSize, &glType, buffer);
			m_uniformTable[std::string(buffer)].type = glType;
			m_uniformTable[std::string(buffer)].size = glSize;
			m_uniformTable[std::string(buffer)].location = glGetUniformLocation(m_id, buffer);
		}
	}

	////////////////////////////////////////////////////////////////
	// Shader tools
	////////////////////////////////////////////////////////////////

	unsigned int CreateShader(const std::string & source, unsigned int type)
	{
		int status;
		char log[1024];
		const char *src_c = source.c_str();

		unsigned int shader_id = glCreateShader(type);
		glShaderSource(shader_id, 1, &src_c, NULL);
		glCompileShader(shader_id);

		glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
		if (!status)
		{
			glGetShaderInfoLog(shader_id, 1024, NULL, log);

			switch (type)
			{
			case GL_VERTEX_SHADER:
				Log::Message("[Shader] Vertex shader compilation error!\n" + std::string(log), Log::ERROR);
				break;
			case GL_GEOMETRY_SHADER:
				Log::Message("[Shader] Geometry shader compilation error!\n" + std::string(log), Log::ERROR);
				break;
			case GL_FRAGMENT_SHADER:
				Log::Message("[Shader] Fragment shader compilation error!\n" + std::string(log), Log::ERROR);
				break;
			default:
				break;
			}

			glDeleteShader(shader_id);
			shader_id = 0;
		}

		return shader_id;
	}

	unsigned int CreateVertexShader(const std::string & source, unsigned int type)
	{
		return CreateShader(source, GL_VERTEX_SHADER);
	}

	unsigned int CreateGeometryShader(const std::string & source, unsigned int type)
	{
		return CreateShader(source, GL_GEOMETRY_SHADER);
	}

	unsigned int CreateFragmentShader(const std::string & source, unsigned int type)
	{
		return CreateShader(source, GL_FRAGMENT_SHADER);
	}

	std::string ReadShaderSource(const std::string& path)
	{
		std::string directory = path.substr(0, path.find_last_of("/\\"));
		std::string source{}, line;
		std::ifstream in(path, std::ios::in);

		if (!in)
		{
			Log::Message("[Shader] Cannot open shader source \"" + path + "\"", Log::ERROR);
			return "";
		}

		while (std::getline(in, line))
		{
			// if we encounter an #include line, include other shader source
			if (line.substr(0, 8) == "#include")
			{
				std::string includePath = directory + "/" + line.substr(9);
				source.append(ReadShaderSource(includePath));
			}
			else
			{
				source.append(line + "\n");
			}
		}

		return source;
	}

	std::string InsertShaderDefine(const std::string & source, const std::vector<std::string>& defines)
	{
		if (defines.size() == 0) return source;

		std::string editedSrc;
		std::string firstLine = source.substr(0, source.find("\n")); // #version xx0 core
		std::string restLines = source.substr(source.find("\n") + 1, source.length() - 1);

		editedSrc.append(firstLine + "\n");

		for (const std::string& define : defines)
		{
			std::string def = "#define " + define + "\n";
			editedSrc.append(def);
		}

		editedSrc.append(restLines);

		return editedSrc;
	}
}