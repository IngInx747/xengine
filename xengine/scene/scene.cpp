#include "scene.h"

#include <graphics/shader_manager.h>
#include <graphics/texture_manager.h>
#include <graphics/material_manager.h>
#include <mesh/mesh_manager.h>
#include <model/model_manager.h>

namespace xengine
{
	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
	}

	void Scene::Initialize()
	{
	}

	void Scene::Clear()
	{
		ModelManager::ClearLocal();
		MeshManager::ClearLocal();
		MaterialManager::ClearLocal();
		TextureManager::ClearLocal();
		ShaderManager::ClearLocal();
	}

	void Scene::Update(float t, float dt)
	{
	}

	////////////////////////////////////////////////////////////////
	// Model
	////////////////////////////////////////////////////////////////

	void Scene::InsertModel(Model* model, bool isStill)
	{
		models.push_back(model);

		if (isStill) stillModels.push_back(model);
		else movingModels.push_back(model);
	}

	void Scene::RemoveModel(Model* model)
	{
		models.erase(std::find(models.begin(), models.end(), model));
		stillModels.erase(std::find(stillModels.begin(), stillModels.end(), model));
		movingModels.erase(std::find(movingModels.begin(), movingModels.end(), model));
	}

	////////////////////////////////////////////////////////////////
	// Light
	////////////////////////////////////////////////////////////////

	void Scene::AddLight(ParallelLight* light)
	{
		parallelLights.push_back(light);
	}

	void Scene::AddLight(PointLight* light)
	{
		pointLights.push_back(light);
	}

	void Scene::AddParticle(ParticleSystem * particle)
	{
		particles.push_back(particle);
	}
}