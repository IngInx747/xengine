#pragma once
#ifndef XE_MESH_TORUS_H
#define XE_MESH_TORUS_H

#include "../mesh.h"

namespace xengine
{
	class Torus : public Mesh
	{
	public:
		Torus(unsigned int xseg, unsigned int yseg, float r1, float r2);
	};
}

#endif // !XE_MESH_TORUS_H
