#pragma once
#ifndef XE_MODEL_H
#define XE_MODEL_H

#include <vector>
#include <memory>

#include <vendor/glm/glm.hpp>
#include <vendor/glm/gtc/quaternion.hpp>

#include <geometry/object.h>
#include <mesh/mesh.h>
#include <graphics/material.h>

namespace xengine
{
	class Model : public GeometryObject
	{
	public:
		Model();
		virtual ~Model();

		Model(const Model& other);
		Model& operator=(const Model& other);

		// update model's and all its children's transform matrices
		virtual void UpdateTransform();

		// update model's and all its children's transform matrices, given a parent transform matrix
		virtual void UpdateTransform(const glm::mat4& parentTransform);

		// insert a mesh with material
		void InsertMesh(const Mesh & mesh, const Material & material);

		// insert a model as child node
		void InsertChild(Model* node);

		// remove a node from this model
		void RemoveChild(Model* node);

		// get all children nodes of this model
		void GetAllNodes(std::vector<Model*>& nodes);

	protected:
		// copy node contents only
		Model(const Model* other);

		// return a deep copy of this model
		Model* deepcopy();

		// get all children nodes of this model
		void getAllNodes(std::vector<Model*>& nodes, bool exclude_this);

		// delete all children nodes from memory (in a deferred way)
		void destoryAllChildren();

		//
		void getHierarchy(const std::vector<Model*>& nodes, std::vector<int>& hierarchy);

	public:
		std::vector<Mesh> meshes;
		std::vector<Material> materials;

		// hierarchical structure
		std::vector<Model*> children;
	};
}

#endif // !XE_MODEL_H
