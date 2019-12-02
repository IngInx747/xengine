#include "scenes.h"

void MyScene2::Initialize()
{
	// models
	glock17 = xengine::ModelManager::LoadFromObj("glock17", "meshes/glock17/Glock_17.obj");
	glock17->SetPosition(glm::vec3(0.0, 0.0, 2.0));
	glock17->SetScale(glm::vec3(0.1f));

	glock17_armed = *glock17;
	glock17_armed.SetPosition(glm::vec3(0.0, 0.0, -2.0));
	glock17_armed.SetScale(glm::vec3(0.1f));

	// environment capture
	xengine::Texture* hdrMap = xengine::TextureManager::LoadHDR("sky env", "textures/backgrounds/colorful_studio.hdr");

	// allocate image-based lighting renderer
	ibl = std::make_shared<xengine::IblRenderer>();
	ibl->GenerateEnvironment(hdrMap);
	irradianceMap = ibl->GetIrradiance();
	reflectionMap = ibl->GetReflection();

	// setup skybox
	skybox.materials[0]->RegisterUniform("lodLevel", 1.5f);
	skybox.SetScale(glm::vec3(1e20f)); // set skybox infinitely big (model size, not the cube)
	skybox.SetCubeMap(ibl->GetEnvironment());

	InsertModel(glock17);
	InsertModel(&glock17_armed);
	InsertModel(&skybox);
}

void MyScene2::Clear()
{
	xengine::ModelManager::ClearScene();
	xengine::MeshManager::ClearScene();
	xengine::MaterialManager::ClearScene();
	xengine::TextureManager::ClearScene();
}

void MyScene2::Update(float t, float dt)
{
	glock17->Rotate(dt, glm::vec3(0, 1, 0));
	glock17_armed.Rotate(dt, glm::vec3(0, 1, 0));
}
