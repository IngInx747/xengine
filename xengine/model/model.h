#pragma once
#ifndef XE_MODEL_H
#define XE_MODEL_H

#include <vector>
#include <memory>

#include <vendor/glm/glm.hpp>
#include <vendor/glm/gtc/quaternion.hpp>

#include <mesh/mesh.h>
#include <graphics/material.h>

#include "object.h"

namespace xengine
{
	class Model : public GeometryObject
	{
	public:
		// update model's and all its children's transform matrices
		virtual void UpdateTransform();

		// update model's and all its children's transform matrices, given a parent transform matrix
		virtual void UpdateTransform(const glm::mat4& parentTransform);

		// insert a mesh with material
		void InsertMesh(Mesh* mesh, Material* material);

		// insert a model as child node
		void InsertChild(Model* node);

		// delete a node from this model node
		void RemoveChild(Model* node);

	public:
		std::vector<Mesh*> meshes;
		std::vector<Material*> materials;

		// hierarchical structure
		std::vector<Model*> children;
	};
}

#endif // !XE_MODEL_H
