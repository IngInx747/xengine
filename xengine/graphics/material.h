#pragma once
#ifndef XE_MATERIAL_H
#define XE_MATERIAL_H

#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include <vendor/glm/glm.hpp>

#include "shader.h"
#include "texture.h"

namespace xengine
{
	class Material
	{
	public:
		enum TYPE
		{
			FORWARD,
			DEFERRED,
			POST_PROCESS,
		};

		enum VAR_TYPE
		{
			BOOL,
			INT,
			FLOAT,
			VEC2,
			VEC3,
			VEC4,
			MAT2,
			MAT3,
			MAT4,
			SAMPLER1D,
			SAMPLER2D,
			SAMPLER3D,
			SAMPLERCUBE,
		};

	public:
		struct Attribute
		{
			// ogl: depth test
			bool bDepthTest;
			bool bDepthWrite;
			unsigned int eDepthFunc;

			// ogl: face culling
			bool bCull;
			unsigned int eCullFace;
			unsigned int eCullWind;

			// ogl: blend
			bool bBlend;
			unsigned int eBlendSrc; // pre-multiplied alpha
			unsigned int eBlendDst;
			unsigned int eBlendEq;

			// shadow state
			bool bShadowCast;
			bool bShadowRecv;

			Attribute();
		};

		struct VarTableEntry
		{
			unsigned int type;

			union
			{
				bool  bVal;
				int   iVal;
				float fVal;

				glm::vec2 vec2;
				glm::vec3 vec3;
				glm::vec4 vec4;
				glm::mat2 mat2;
				glm::mat3 mat3;
				glm::mat4 mat4;
			};

			VarTableEntry() {}
			VarTableEntry(const VarTableEntry& entry) : type(entry.type), mat4(entry.mat4) {}
			~VarTableEntry() {}
		};

		struct TexTableEntry
		{
			unsigned int unit;
			Texture* texture;
		};

	public:
		Material(Shader* shader);

		// register a texture that sent to shader before rendering (unit allocated automatically)
		void RegisterTexture(const std::string& name, Texture* texture);

		// register a parameter that sent to shader before rendering
		void RegisterUniform(const std::string& name, bool value);
		void RegisterUniform(const std::string& name, int value);
		void RegisterUniform(const std::string& name, float value);
		void RegisterUniform(const std::string& name, const glm::vec2& value);
		void RegisterUniform(const std::string& name, const glm::vec3& value);
		void RegisterUniform(const std::string& name, const glm::vec4& value);
		void RegisterUniform(const std::string& name, const glm::mat2& value);
		void RegisterUniform(const std::string& name, const glm::mat3& value);
		void RegisterUniform(const std::string& name, const glm::mat4& value);

		// on render methods
		void UpdateOglStatus(); // update opengl status
		void UpdateShaderUniforms(); // flush uniforms into shader before rendering

	public:
		unsigned int type;

		// OpenGL render config
		Attribute attribute;

		// corresponding shader
		Shader* shader;

	private:
		// shader uniforms table (better store model-dependent but time-irrelevant params)
		std::unordered_map<std::string, VarTableEntry> uniformTable;

		// texture register table
		std::unordered_map<std::string, TexTableEntry> textureTable;

		// texture indices manager
		std::unordered_set<unsigned int> texIndicesSet;
	};
}

#endif // !XE_MATERIAL_H
