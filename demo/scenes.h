#pragma once
#ifndef SCENES_H
#define SCENES_H

#include <xengine.h>

class MyScene1 : public xengine::Scene
{
public:
	void Initialize();
	void Clear();
	void Update(float t, float dt);

public:
	// models
	xengine::Model torus_0;
	xengine::Model torus_1;
	xengine::Model torus_2;
	xengine::Model plasmaOrb;
	xengine::Model* sponza;
	xengine::Skybox skybox;

	// lights
	xengine::ParallelLight dir_light;
	std::vector<xengine::PointLight> torchLights;

	// ibl
	std::shared_ptr<xengine::IblRenderer> ibl;
};

class MyScene2 : public xengine::Scene
{
public:
	void Initialize();
	void Clear();
	void Update(float t, float dt);

public:
	// models
	xengine::Model* glock17;
	xengine::Model glock17_armed;
	xengine::Skybox skybox;
	xengine::Model floor;

	// lights
	xengine::ParallelLight dir_light;

	//
	xengine::ParticleSystem firework;

	// ibl
	std::shared_ptr<xengine::IblRenderer> ibl;
};

#endif // !SCENES_H
