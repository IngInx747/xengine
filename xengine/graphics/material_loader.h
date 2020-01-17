#pragma once
#ifndef XE_MATERIAL_LOADER_H
#define XE_MATERIAL_LOADER_H

#include "material.h"

struct aiMaterial;
struct aiScene;
struct aiMesh;

namespace xengine
{
	// API for model manager to load mesh from model file (wave-front format)
	Material LoadMaterial_Impl_Assimp(aiMaterial* aMaterial, const std::string& directory, aiMesh* aMesh);
}

#endif // !XE_MATERIAL_LOADER_H
