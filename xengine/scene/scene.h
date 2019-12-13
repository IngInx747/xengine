#pragma once
#ifndef XE_SCENE_H
#define XE_SCENE_H

#include <vector>
#include <memory>
#include <unordered_set>

#include <utility/counter.h>
#include <graphics/texture/texture.h>
#include <graphics/material/material.h>
#include <mesh/mesh.h>
#include <model/model.h>
#include <graphics/light/point_light.h>
#include <graphics/light/parallel_light.h>
#include <graphics/renderer/ibl_renderer.h>
#include <graphics/particle_system/particle_system.h>

namespace xengine
{
	class Scene
	{
	public:
		Scene();

		virtual void Initialize() {}
		virtual void Clear() {}
		virtual void Update(float t, float dt) {}

		// model
		void InsertModel(Model* model, bool isStill = false);
		void RemoveModel(Model* model);

		// light
		void AddLight(ParallelLight* light);
		void AddLight(PointLight* light);

		// particle
		void AddParticle(ParticleSystem* particle);

	private:

	public:
		// all models in the scene
		std::vector<Model*> models;

		// TODO
		std::vector<Model*> stillModels;
		std::vector<Model*> movingModels;

		// lights
		std::vector<ParallelLight*> parallelLights;
		std::vector<PointLight*> pointLights;

		// particle
		std::vector<ParticleSystem*> particles;

		// ambient (IBL) (bad practice to put a big module in general scene class)
		CubeMap* irradianceMap;
		CubeMap* reflectionMap;
	};
}

#endif // !XE_SCENE_H
