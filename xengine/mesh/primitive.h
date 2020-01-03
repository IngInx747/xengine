#pragma once
#ifndef XE_MESH_PRIMITIVE_H
#define XE_MESH_PRIMITIVE_H

#include "mesh.h"

namespace xengine
{
	class Quad : public Mesh
	{
	public:
		Quad();
		Quad(float width, float height);
	};

	class Plane : public Mesh
	{
	public:
		Plane();
		//Plane(unsigned int xseg, unsigned int yseg);
	};

	class Cube : public Mesh
	{
	public:
		Cube();
	};

	class Sphere : public Mesh
	{
	public:
		Sphere(unsigned int xseg, unsigned int yseg);
	};

	class Torus : public Mesh
	{
	public:
		Torus(unsigned int xseg, unsigned int yseg, float r1, float r2);
	};
}

#endif // !XE_MESH_PRIMITIVE_H
