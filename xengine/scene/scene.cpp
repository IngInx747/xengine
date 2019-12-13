#include "scene.h"

namespace xengine
{
	Scene::Scene()
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