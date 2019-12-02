#pragma once
#ifndef XE_MESH_CIRCLE_H
#define XE_MESH_CIRCLE_H

#include "mesh.h"

namespace xengine
{
	class Circle : public Mesh
	{
	public:
		Circle(unsigned int pseg, unsigned int rseg);
	};
}

#endif // !XE_MESH_CIRCLE_H
