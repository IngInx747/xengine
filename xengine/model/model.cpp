#include "model.h"

#include <vendor/glm/glm.hpp>
#include <vendor/glm/gtc/matrix_transform.hpp>

namespace xengine
{
	void Model::InsertMesh(Mesh * mesh, Material * material)
	{
		meshes.push_back(mesh);
		materials.push_back(material);
		aabbLocal.UnionAABB(mesh->Aabb());
	}

	void Model::InsertChild(Model* node)
	{
		node->dirty = true;
		children.push_back(node);
		// Note: child won't affect parent's bounding box
	}

	void Model::RemoveChild(Model* node)
	{
		auto it = std::find(children.begin(), children.end(), node);

		if (it != children.end())
		{
			node->dirty = true;
			children.erase(it);
			// Note: child won't affect parent's bounding box
		}
	}

	void Model::UpdateTransform()
	{
		UpdateTransform(glm::mat4{});
	}

	void Model::UpdateTransform(const glm::mat4 & parentTransform)
	{
		bool modified = dirty;

		GeometryObject::UpdateTransform(parentTransform);

		for (unsigned int i = 0; i < children.size(); ++i)
		{
			// child must be dirty if parent was modified
			if (modified) children[i]->dirty = true;

			// scan whole tree to see if there is any dirty node
			children[i]->UpdateTransform(transform);
		}
	}
}