#pragma once
#ifndef XE_MESH_SPHERE_H
#define XE_MESH_SPHERE_H

#include "../mesh.h"

namespace xengine
{
	class Sphere : public Mesh
	{
	public:
		Sphere(unsigned int xseg, unsigned int yseg);
	};
}

#endif // !XE_MESH_SPHERE_H
