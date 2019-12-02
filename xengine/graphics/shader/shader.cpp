#include "shader.h"

#include <vendor/glad/glad.h>
#include <vendor/glm/glm.hpp>

#include <utility/log.h>

namespace xengine
{
	Shader::Shader()
	{
	}

	Shader::~Shader()
	{
		if (m_id)
		{
			glDeleteProgram(m_id);
			m_id = 0;
		}
	}

	void Shader::Use()
	{
		glUseProgram(m_id);
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
		if (loc >= 0) glUniform1i(loc, (int)value);
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

	void Shader::GenerateVertFragShader(const std::string & vsCode, const std::string & fsCode, const std::vector<std::string>& defines)
	{
		// compile both shaders and link them
		int status;
		char log[1024];
		this->m_id = glCreateProgram();
		unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
		unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);

		// if a list of define statements is specified, add these to the start of the shader 
		// source, s.t. we can selectively compile different shaders based on the defines we set.
		if (defines.size() > 0)
		{
			std::vector<std::string> vsMergedCode;
			std::vector<std::string> fsMergedCode;
			std::string vsCodeTmp(vsCode);
			std::string fsCodeTmp(fsCode);

			std::string firstLine = vsCode.substr(0, vsCode.find("\n"));
			if (firstLine.find("#version") != std::string::npos)
			{
				// strip shader code of first line and add to list of shader code strings.
				vsCodeTmp = vsCode.substr(vsCode.find("\n") + 1, vsCode.length() - 1);
				vsMergedCode.push_back(firstLine + "\n");
			}

			firstLine = fsCode.substr(0, fsCode.find("\n"));
			if (firstLine.find("#version") != std::string::npos)
			{
				// strip shader code of first line and add to list of shader code strings.
				fsCodeTmp = fsCode.substr(fsCode.find("\n") + 1, fsCode.length() - 1);
				fsMergedCode.push_back(firstLine + "\n");
			}

			// then add define statements to the shader string list.
			for (unsigned int i = 0; i < defines.size(); ++i)
			{
				std::string define = "#define " + defines[i] + "\n";
				vsMergedCode.push_back(define);
				fsMergedCode.push_back(define);
			}

			// then add remaining shader code to merged result and pass result to glShaderSource.
			vsMergedCode.push_back(vsCodeTmp);
			fsMergedCode.push_back(fsCodeTmp);

			// note that we manually build an array of C style strings as glShaderSource doesn't 
			// expect it in any other format.
			// all strings are null-terminated so pass NULL as glShaderSource's final argument.
			const char **vsStringsC = new const char*[vsMergedCode.size()];
			const char **fsStringsC = new const char*[fsMergedCode.size()];

			for (unsigned int i = 0; i < vsMergedCode.size(); ++i)
				vsStringsC[i] = vsMergedCode[i].c_str();

			for (unsigned int i = 0; i < fsMergedCode.size(); ++i)
				fsStringsC[i] = fsMergedCode[i].c_str();

			glShaderSource(vs, (GLsizei)vsMergedCode.size(), vsStringsC, NULL);
			glShaderSource(fs, (GLsizei)fsMergedCode.size(), fsStringsC, NULL);

			delete[] vsStringsC;
			delete[] fsStringsC;
		}
		else
		{
			const char *vsSourceC = vsCode.c_str();
			const char *fsSourceC = fsCode.c_str();
			glShaderSource(vs, 1, &vsSourceC, NULL);
			glShaderSource(fs, 1, &fsSourceC, NULL);
		}

		glCompileShader(vs);
		glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
		if (!status)
		{
			glGetShaderInfoLog(vs, 1024, NULL, log);
			Log::Message("[Shader] Vertex shader compilation error!\n" + std::string(log), Log::ERROR);
		}
		glAttachShader(m_id, vs);

		glCompileShader(fs);
		glGetShaderiv(fs, GL_COMPILE_STATUS, &status);
		if (!status)
		{
			glGetShaderInfoLog(fs, 1024, NULL, log);
			Log::Message("[Shader] Fragment shader compilation error!\n" + std::string(log), Log::ERROR);
		}
		glAttachShader(m_id, fs);

		glLinkProgram(m_id);
		glGetProgramiv(m_id, GL_LINK_STATUS, &status);
		if (!status)
		{
			glGetProgramInfoLog(m_id, 1024, NULL, log);
			Log::Message("[Shader] Shader program linking error: \n" + std::string(log), Log::ERROR);
		}

		glDeleteShader(vs);
		glDeleteShader(fs);

		// query the number of active uniforms and attributes
		int nrAttributes, nrUniforms;
		glGetProgramiv(m_id, GL_ACTIVE_ATTRIBUTES, &nrAttributes);
		glGetProgramiv(m_id, GL_ACTIVE_UNIFORMS, &nrUniforms);

		// iterate over all active attributes
		char buffer[128];

		for (int i = 0; i < nrAttributes; ++i)
		{
			GLenum glType;
			GLsizei glSize;
			glGetActiveUniform(m_id, i, sizeof(buffer), 0, &glSize, &glType, buffer);
			m_attributeTable[std::string(buffer)].type = glType;
			m_attributeTable[std::string(buffer)].size = glSize;
			m_attributeTable[std::string(buffer)].location = glGetUniformLocation(m_id, buffer);
		}

		// iterate over all active uniforms
		for (int i = 0; i < nrUniforms; ++i)
		{
			GLenum glType;
			GLsizei glSize;
			glGetActiveUniform(m_id, i, sizeof(buffer), 0, &glSize, &glType, buffer);
			m_uniformTable[std::string(buffer)].type = glType;
			m_uniformTable[std::string(buffer)].size = glSize;
			m_uniformTable[std::string(buffer)].location = glGetUniformLocation(m_id, buffer);
		}
	}
}