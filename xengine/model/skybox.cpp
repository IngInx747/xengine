#include "skybox.h"

#include <vendor/glad/glad.h>
#include <vendor/glm/glm.hpp>

#include <mesh/mesh_manager.h>
#include <graphics/material_manager.h>

namespace xengine
{
	Skybox::Skybox() : Model()
	{
		meshes.resize(1);
		materials.resize(1);

		meshes[0] = MeshManager::LoadPrimitive("cube");
		materials[0] = MaterialManager::Get("skybox");
	}

	void Skybox::SetCubeMap(const CubeMap& cubemap)
	{
		materials[0]->RegisterTexture("background", cubemap);
	}
}