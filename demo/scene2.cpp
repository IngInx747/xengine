#include "scenes.h"

void MyScene2::Initialize()
{
	xengine::Mesh* plane = xengine::MeshManager::LoadPrimitive("plane");
	xengine::Material* mtrPlane = xengine::MaterialManager::Get("deferred");

	// models
	glock17 = xengine::ModelManager::LoadFromObj("glock17", "meshes/glock17/Glock_17.obj");
	glock17->SetPosition(glm::vec3(0.0, 0.0, 2.0));
	glock17->SetScale(glm::vec3(0.1f));

	glock17_armed = *glock17;
	glock17_armed.SetPosition(glm::vec3(0.0, 0.0, -2.0));
	glock17_armed.SetScale(glm::vec3(0.1f));

	floor.InsertMesh(plane, mtrPlane);
	floor.SetPosition(glm::vec3(0.0, -2.0, 0.0));
	floor.SetScale(glm::vec3(10.0f));

	// environment capture
	xengine::Texture* hdrMap = xengine::TextureManager::LoadHDR("sky env", "textures/backgrounds/colorful_studio.hdr");

	// allocate image-based lighting renderer
	fbEnvironment = xengine::IblRenderer::CreateEnvironment(hdrMap);
	xengine::CubeMap* envMap = fbEnvironment.GetColorAttachment(0);

	fbIrradiance = xengine::IblRenderer::CreateIrradiance(envMap);
	irradianceMap = fbIrradiance.GetColorAttachment(0);

	fbReflection = xengine::IblRenderer::CreateReflection(envMap);
	reflectionMap = fbReflection.GetColorAttachment(0);

	// setup skybox
	skybox.materials[0]->RegisterUniform("lodLevel", 1.5f);
	skybox.SetScale(glm::vec3(1e20f)); // set skybox infinitely big (model size, not the cube)
	skybox.SetCubeMap(envMap);

	// light
	dir_light.direction = glm::vec3(0.0f, -1.0f, 0.0f);
	dir_light.color = glm::vec3(1.0f, 1.0f, 1.0f);
	dir_light.intensity = 10.0f;
	dir_light.UpdateShadowView(glm::vec3(0, -3, 0));

	// particle
	firework.Initialize();

	InsertModel(&floor);
	InsertModel(glock17);
	InsertModel(&glock17_armed);
	InsertModel(&skybox);

	AddLight(&dir_light);

	AddParticle(&firework);
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
	firework.SetPosition(glm::vec3(std::cosf(t), 0, std::sinf(t)) * 1.0f);
}
