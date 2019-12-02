#include "model.h"

#include <vendor/glm/glm.hpp>
#include <vendor/glm/gtc/matrix_transform.hpp>

namespace xengine
{
	Model::Transform::Transform()
		:
		position(0.0f),
		scale(1.0f),
		rotation(glm::quat{})
	{}

	//Model::Model()
	//{}
	//
	//Model::Model(const Model & other)
	//{
	//}

	void Model::InsertMesh(Mesh * mesh, Material * material)
	{
		meshes.push_back(mesh);
		materials.push_back(material);
		aabbLocal.UnionAABB(mesh->aabb);
	}

	void Model::InsertChild(Model* node)
	{
		node->dirty = true;
		children.push_back(node);
		aabbLocal.UnionAABB(node->aabbLocal);
	}

	void Model::RemoveChild(Model* node)
	{
		auto it = std::find(children.begin(), children.end(), node);

		if (it != children.end())
		{
			node->dirty = true;
			children.erase(it);
			// TODO: update aabb
		}
	}

	void Model::SetPosition(const glm::vec3& position)
	{
		transform.position = position;
		dirty = true;
	}

	void Model::SetRotation(float angle, const glm::vec3& axis)
	{
		transform.rotation = glm::rotate(glm::quat{}, angle, axis);
		dirty = true;
	}

	void Model::SetScale(const glm::vec3& scale)
	{
		transform.scale = scale;
		dirty = true;
	}

	void Model::Move(const glm::vec3& displace)
	{
		transform.position += displace;
		dirty = true;
	}

	void Model::Rotate(float angle, const glm::vec3& axis)
	{
		transform.rotation = glm::rotate(transform.rotation, angle, axis);
		dirty = true;
	}

	void Model::Scale(const glm::vec3& scale)
	{
		transform.scale *= scale;
		dirty = true;
	}

	glm::vec3 Model::GetWorldPosition()
	{
		glm::vec4 pos = transform.transform * glm::vec4(transform.position, 1.0f);
		return glm::vec3(pos);
	}

	glm::vec3 Model::GetWorldScale()
	{
		glm::mat4 matT{ transform.transform };
		glm::vec3 vecS{ matT[0][0], matT[1][1], matT[2][2] };
		if (vecS.x < 0.0f) vecS.x = -vecS.x;
		if (vecS.y < 0.0f) vecS.y = -vecS.y;
		if (vecS.z < 0.0f) vecS.z = -vecS.z;
		return vecS;
	}

	glm::mat4 Model::GetTransform()
	{
		if (dirty) UpdateTransform();
		return transform.transform;
	}

	void Model::UpdateTransform()
	{
		// the node is treated as root, so a identity matrix is passed
		UpdateTransform(glm::mat4{});
	}

	void Model::UpdateTransform(const glm::mat4 & parentTransform)
	{
		transform.prevTransform = transform.transform;

		// update self if modified previously
		if (dirty)
		{
			// first scale, then rotate, then translation
			glm::mat4 translate = glm::translate(glm::mat4{}, transform.position);
			glm::mat4 scale = glm::scale(glm::mat4{}, transform.scale);
			glm::mat4 rotation = glm::mat4_cast(transform.rotation);
			transform.transform = translate * rotation * scale;

			// transmit transform from parent
			transform.transform = parentTransform * transform.transform;

			// update bounding box
			aabbGlobal.BuildFromTransform(aabbLocal, transform.transform); // update global box
		}

		for (unsigned int i = 0; i < children.size(); ++i)
		{
			// child must be dirty if parent was modified
			if (dirty) children[i]->dirty = true;

			// scan whole tree to see if there is any dirty node
			children[i]->UpdateTransform(transform.transform);
		}

		dirty = false;
	}
}