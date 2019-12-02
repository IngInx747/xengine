#pragma once
#ifndef XE_MESH_QUAD_H
#define XE_MESH_QUAD_H

#include "../mesh.h"

namespace xengine
{
	class Quad : public Mesh
	{
	public:
		Quad();
		Quad(float width, float height);
	};
}

#endif // !XE_MESH_QUAD_H
