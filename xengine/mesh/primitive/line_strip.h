#pragma once
#ifndef XE_MESH_LINE_STRIP_H
#define XE_MESH_LINE_STRIP_H

#include "mesh.h"

namespace xengine
{
	class LineStrip : public Mesh
	{
	public:
		LineStrip(float width, unsigned int seg);
	};
}

#endif // !XE_MESH_LINE_STRIP_H
