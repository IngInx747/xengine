#include "model.h"

#include <vendor/glm/glm.hpp>
#include <vendor/glm/gtc/matrix_transform.hpp>

namespace xengine
{
	Model::Model()
	{
	}

	Model::~Model()
	{
		// to avoid calling this function recursively
		if (!children.empty())
			destoryAllChildren();
	}

	Model::Model(const Model & other)
		:
		GeometryObject(other),
		meshes(other.meshes),
		materials(other.materials)
	{
		for (Model* child : other.children)
		{
			Model* node = child->deepcopy();
			InsertChild(node);
		}
	}

	Model & Model::operator=(const Model & other)
	{
		GeometryObject::operator=(other);
		meshes = other.meshes;
		materials = other.materials;

		for (Model* child : other.children)
		{
			Model* node = child->deepcopy();
			InsertChild(node);
		}

		return *this;
	}

	Model::Model(const Model * other)
		:
		GeometryObject(*other),
		meshes(other->meshes),
		materials(other->materials)
	{
		// copy node contents only
		// class private method
		// other must not be null

		children.reserve(other->children.size());
	}

	void Model::getAllNodes(std::vector<Model*>& nodes, bool exclude_this)
	{
		std::vector<Model*> recStack;

		if (exclude_this)
		{
			for (Model* child : children)
				recStack.push_back(child);
		}
		else
		{
			recStack.push_back(this);
		}

		while (!recStack.empty())
		{
			Model* node = recStack.back();
			recStack.pop_back();

			nodes.push_back(node);

			for (auto child : node->children)
				recStack.push_back(child);
		}
	}

	Model* Model::deepcopy()
	{
		std::vector<Model*> nodes, copy_nodes;
		std::vector<int> hierarchy;

		getAllNodes(nodes, false);
		copy_nodes.reserve(nodes.size());

		// firstly, copy node contents
		for (Model* node : nodes)
		{
			Model* copy_node = new Model(node); // private constructor
			copy_nodes.push_back(copy_node);
		}

		// secondly, copy hierarchical structure
		getHierarchy(nodes, hierarchy);

		for (int i = 0; i < copy_nodes.size(); ++i)
		{
			int parent_id = hierarchy[i];

			if (parent_id != -1)
				copy_nodes[parent_id]->InsertChild(copy_nodes[i]);
		}

		return copy_nodes[0];
	}

	void Model::destoryAllChildren()
	{
		std::vector<Model*> nodes;

		getAllNodes(nodes, true);

		for (Model* node : nodes)
		{
			node->children.clear(); // to avoid calling this function recursively at deleting stage
			delete node;
		}
	}

	// input:  [ ... p ... c0 ... c1 ... c2 ... ] where the index of parent p is ip
	// output: [ ... * ... ip ... ip ... ip ... ]
	// if node has no parent then corresponding entity in the output is -1
	void Model::getHierarchy(const std::vector<Model*>& nodes, std::vector<int>& hierarchy)
	{
		std::unordered_map<Model*, int> parent_indices;

		hierarchy.resize(nodes.size(), -1);

		for (int i = 0; i < nodes.size(); ++i)
		{
			for (Model* node : nodes[i]->children)
			{
				parent_indices[node] = i;
			}
		}

		for (int i = 0; i < nodes.size(); ++i)
		{
			auto it = parent_indices.find(nodes[i]);
			if (it != parent_indices.end()) hierarchy[i] = it->second;
		}
	}

	void Model::GetAllNodes(std::vector<Model*>& nodes)
	{
		getAllNodes(nodes, false);
	}

	void Model::InsertMesh(const Mesh & mesh, const Material & material)
	{
		meshes.push_back(mesh);
		materials.push_back(material);
		aabbLocal.UnionAABB(mesh.Aabb());
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
		if (dirty)
		{
			for (unsigned int i = 0; i < children.size(); ++i)
			{
				// child must be dirty if parent was modified
				children[i]->dirty = true;
			}
		}

		GeometryObject::UpdateTransform(parentTransform);

		// scan whole tree to see if there is any dirty node
		for (unsigned int i = 0; i < children.size(); ++i)
		{
			children[i]->UpdateTransform(transform);
		}
	}

#if 0
	// A non-recursive UpdateTransform version
	// deprecated for bad performance

	void Model::UpdateTransform()
	{
		std::vector<Model*> nodes;
		std::vector<int> hierarchy;

		getAllNodes(nodes, false);
		getHierarchy(nodes, hierarchy);

		for (int i = 0; i < nodes.size(); ++i)
		{
			if (nodes[i]->dirty)
			{
				for (Model* child : nodes[i]->children)
					child->dirty = true;
			}

			if (hierarchy[i] != -1)
			{
				Model* parent = nodes[hierarchy[i]];
				nodes[i]->GeometryObject::UpdateTransform(parent->transform);
			}
			else // no parent
			{
				nodes[i]->GeometryObject::UpdateTransform();
			}
		}
	}
#endif
}