#pragma once
#ifndef XE_MODEL_H
#define XE_MODEL_H

#include <vector>
#include <memory>

#include <vendor/glm/glm.hpp>
#include <vendor/glm/gtc/quaternion.hpp>

#include <geometry/aabb.h>
#include <mesh/mesh.h>
#include <graphics/material/material.h>

namespace xengine
{
	class Model
	{
	public:
		struct Transform
		{
			glm::mat4 transform;
			glm::mat4 prevTransform;
			glm::quat rotation;
			glm::vec3 position;
			glm::vec3 scale;

			Transform();
		};

	public:
		//Model();
		//Model(const Model& other);

		// set transform
		void SetPosition(const glm::vec3& position);
		void SetRotation(float angle, const glm::vec3& axis);
		void SetScale(const glm::vec3& scale);

		// get transform
		glm::vec3 GetWorldPosition();
		glm::vec3 GetWorldScale();
		glm::mat4 GetTransform();

		// make a displacement based on current position
		void Move(const glm::vec3& position);

		// make a rotation based on current posture
		void Rotate(float angle, const glm::vec3& axis);

		// make a scaling based on current size
		void Scale(const glm::vec3& scale);

		// update model's and all its children's transform matrices
		void UpdateTransform();

		// update model's and all its children's transform matrices, given a parent transform matrix
		void UpdateTransform(const glm::mat4& parentTransform);

		// insert a mesh with material
		void InsertMesh(Mesh* mesh, Material* material);

		// insert a model as child node
		void InsertChild(Model* node);

		// delete a node from this model node
		void RemoveChild(Model* node);

	public:
		std::vector<Mesh*> meshes;
		std::vector<Material*> materials;

		// bounding box
		AABB aabbLocal;
		AABB aabbGlobal;

		// transform
		Transform transform;
		bool dirty;

		// hierarchical structure
		std::vector<Model*> children;
	};
}

#endif // !XE_MODEL_H
