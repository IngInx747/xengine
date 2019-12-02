#pragma once
#ifndef XE_SHADER_H
#define XE_SHADER_H

#include <string>
#include <vector>
#include <unordered_map>

#include <vendor/glm/common.hpp>

namespace xengine
{
	class Shader
	{
	public:
		struct VarTableEntry
		{
			unsigned int type;
			unsigned int size;
			unsigned int location;
		};

	public:
		Shader();
		~Shader();

		// make this shader as current
		void Use();

		inline unsigned int ID() const { return m_id; }

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

		// generate vertex-fragment shader program
		void GenerateVertFragShader(const std::string& vsCode, const std::string& fsCode, const std::vector<std::string>& defines);

	protected:
		unsigned int m_id;
		std::unordered_map<std::string, VarTableEntry> m_attributeTable;
		std::unordered_map<std::string, VarTableEntry> m_uniformTable;
	};
}

#endif // !XE_SHADER_H
