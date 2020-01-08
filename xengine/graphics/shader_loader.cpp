#include "shader_loader.h"

#include <string>
#include <fstream>

#include <vendor/glad/glad.h>

#include <utility/log.h>

namespace xengine
{
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
				Log::Message("[ShaderLoader] Vertex shader compilation error!\n" + std::string(log), Log::ERROR);
				break;
			case GL_GEOMETRY_SHADER:
				Log::Message("[ShaderLoader] Geometry shader compilation error!\n" + std::string(log), Log::ERROR);
				break;
			case GL_FRAGMENT_SHADER:
				Log::Message("[ShaderLoader] Fragment shader compilation error!\n" + std::string(log), Log::ERROR);
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
			Log::Message("[ShaderLoader] Cannot open shader source \"" + path + "\"", Log::ERROR);
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

	Shader LoadShaderVF(
		const std::string& vsPath,
		const std::string& fsPath,
		const std::vector<std::string>& defines)
	{
		Shader shader;

		shader.AttachVertexShader(InsertShaderDefine(ReadShaderSource(vsPath), defines));
		shader.AttachFragmentShader(InsertShaderDefine(ReadShaderSource(fsPath), defines));

		shader.GenerateAndLink();

		return shader;
	}

	Shader LoadShaderVGF(
		const std::string & vsPath,
		const std::string & gsPath,
		const std::string & fsPath,
		const std::vector<std::string>& defines)
	{
		Shader shader;

		shader.AttachVertexShader(InsertShaderDefine(ReadShaderSource(vsPath), defines));
		shader.AttachGeometryShader(InsertShaderDefine(ReadShaderSource(gsPath), defines));
		shader.AttachFragmentShader(InsertShaderDefine(ReadShaderSource(fsPath), defines));

		shader.GenerateAndLink();

		return shader;
	}
}