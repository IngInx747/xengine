#include "material.h"

#include <vendor/glad/glad.h>
#include <vendor/glm/glm.hpp>

#include <utility/log.h>

#include "../platform/ogl_status.h"

namespace xengine
{
	////////////////////////////////////////////////////////////////
	// Material attribute
	////////////////////////////////////////////////////////////////

	Material::Attribute::Attribute()
	{
		bDepthTest = GL_TRUE;
		bDepthWrite = GL_TRUE;
		eDepthFunc = GL_LESS;
		bCull = GL_TRUE;
		eCullFace = GL_BACK;
		eCullWind = GL_CCW;
		bBlend = GL_FALSE;
		eBlendSrc = GL_ONE;
		eBlendDst = GL_ONE_MINUS_SRC_ALPHA;
		eBlendEq = GL_FUNC_ADD;
		bShadowCast = true;
		bShadowRecv = true;
	}

	////////////////////////////////////////////////////////////////
	// Material
	////////////////////////////////////////////////////////////////

	Material::Material(Shader* shader)
		:
		type(TYPE::FORWARD),
		shader(shader)
	{}

	void Material::RegisterTexture(const std::string& name, Texture* texture)
	{
		auto itn = textureTable.find(name);

		// if texture name is never met, assign a unique texture unit to it
		if (itn == textureTable.end())
		{
			unsigned int unit = 0;
			unsigned int maxUnit = static_cast<unsigned int>(OglStatus::GetMaxTextureUnit());

			// search for a available slot for the texture
			for (; texIndicesSet.find(unit) != texIndicesSet.end(); ++unit) {}

			if (unit >= maxUnit)
				Log::Message("[Material] Number of textures exceeds maximum: " + std::to_string(maxUnit), Log::WARN);

			// record unit as occupied
			texIndicesSet.insert(unit);

			textureTable[name].unit = unit;
		}

		textureTable[name].texture = texture;
	}

	void Material::RegisterUniform(const std::string& name, bool value)
	{
		uniformTable[name].type = VAR_TYPE::BOOL;
		uniformTable[name].bVal = value;
	}

	void Material::RegisterUniform(const std::string& name, int value)
	{
		uniformTable[name].type = VAR_TYPE::INT;
		uniformTable[name].iVal = value;
	}

	void Material::RegisterUniform(const std::string& name, float value)
	{
		uniformTable[name].type = VAR_TYPE::FLOAT;
		uniformTable[name].fVal = value;
	}

	void Material::RegisterUniform(const std::string& name, const glm::vec2& value)
	{
		uniformTable[name].type = VAR_TYPE::VEC2;
		uniformTable[name].vec2 = value;
	}

	void Material::RegisterUniform(const std::string& name, const glm::vec3& value)
	{
		uniformTable[name].type = VAR_TYPE::VEC3;
		uniformTable[name].vec3 = value;
	}

	void Material::RegisterUniform(const std::string& name, const glm::vec4& value)
	{
		uniformTable[name].type = VAR_TYPE::VEC4;
		uniformTable[name].vec4 = value;
	}

	void Material::RegisterUniform(const std::string& name, const glm::mat2& value)
	{
		uniformTable[name].type = VAR_TYPE::MAT2;
		uniformTable[name].mat2 = value;
	}

	void Material::RegisterUniform(const std::string& name, const glm::mat3& value)
	{
		uniformTable[name].type = VAR_TYPE::MAT3;
		uniformTable[name].mat3 = value;
	}

	void Material::RegisterUniform(const std::string& name, const glm::mat4& value)
	{
		uniformTable[name].type = VAR_TYPE::MAT4;
		uniformTable[name].mat4 = value;
	}

	void Material::UpdateOglStatus()
	{
		OglStatus::SetDepthFunc(attribute.eDepthFunc);
		OglStatus::SetDepthTest(attribute.bDepthTest);

		OglStatus::SetCull(attribute.bCull);
		OglStatus::SetCullFace(attribute.eCullFace);

		OglStatus::SetBlend(attribute.bBlend);
		OglStatus::SetBlendFunc(attribute.eBlendSrc, attribute.eBlendDst);
	}

	void Material::UpdateShaderUniforms()
	{
		shader->Use();

		for (const auto& mp : textureTable)
		{
			if (!mp.second.texture) continue;

			mp.second.texture->Bind(mp.second.unit);
			shader->SetUniform(mp.first, mp.second.unit);
		}

		for (const auto& mp : uniformTable)
		{
			switch (mp.second.type)
			{
			case Material::BOOL:
				shader->SetUniform(mp.first, mp.second.bVal);
				break;
			case Material::INT:
				shader->SetUniform(mp.first, mp.second.iVal);
				break;
			case Material::FLOAT:
				shader->SetUniform(mp.first, mp.second.fVal);
				break;
			case Material::VEC2:
				shader->SetUniform(mp.first, mp.second.vec2);
				break;
			case Material::VEC3:
				shader->SetUniform(mp.first, mp.second.vec3);
				break;
			case Material::VEC4:
				shader->SetUniform(mp.first, mp.second.vec4);
				break;
			case Material::MAT2:
				shader->SetUniform(mp.first, mp.second.mat2);
				break;
			case Material::MAT3:
				shader->SetUniform(mp.first, mp.second.mat3);
				break;
			case Material::MAT4:
				shader->SetUniform(mp.first, mp.second.mat4);
				break;
			default:
				break;
			}
		}
	}
}