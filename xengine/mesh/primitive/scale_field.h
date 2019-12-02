#pragma once
#ifndef XE_MESH_SCALE_FIELD_H
#define XE_MESH_SCALE_FIELD_H

#include "mesh.h"

namespace xengine
{
	class ScaleField : public Mesh
	{
	public:
		// Generate 3D mesh from SDF
		ScaleField(std::function<float(glm::vec3)>& sdf, float maxDist, unsigned short gridResolution);
	};
}

#endif // !XE_MESH_SCALE_FIELD_H
