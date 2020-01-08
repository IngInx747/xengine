#pragma once
#ifndef XE_MESH_LOADER_H
#define XE_MESH_LOADER_H

#include "mesh.h"

struct aiMesh;

namespace xengine
{
	Mesh LoadMesh_Primitive(const std::string & type, const std::vector<double>& params);

	Mesh LoadMesh_Impl_Assimp(aiMesh* aMesh);
}

#endif // !XE_MESH_LOADER_H
