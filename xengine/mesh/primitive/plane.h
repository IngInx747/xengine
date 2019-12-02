#pragma once
#ifndef XE_MESH_PLANE_H
#define XE_MESH_PLANE_H

#include "../mesh.h"

namespace xengine
{
	class Plane : public Mesh
	{
	public:
		Plane();
		//Plane(unsigned int xseg, unsigned int yseg);
	};
}

#endif // !XE_MESH_PLANE_H
