#include "scenes.h"

void MyScene1::Initialize()
{
	// mesh
	xengine::Mesh plane = xengine::MeshManager::LoadPrimitive("plane");
	xengine::Mesh sphere = xengine::MeshManager::LoadPrimitive("sphere", 256, 128);
	xengine::Mesh torus = xengine::MeshManager::LoadPrimitive("torus", 32, 32, 2.0f, 0.5f);

	// shader
	xengine::Shader shdPlasma = xengine::ShaderManager::LoadLocalVF("plasma orb", "shaders/plasma_orb.vs", "shaders/plasma_orb.fs");

	// material
	xengine::Material* mtrMetal = xengine::MaterialManager::Get("deferred no TBN"); // "normal debug" "deferred"
	mtrMetal->RegisterTexture("TexMetallic", xengine::TextureManager::Get("white"));

	//xengine::Material* mtrTorus = xengine::MaterialManager::Get("deferred"); // "normal debug" "deferred"
	xengine::Material* mtrTorus = xengine::MaterialManager::Get("deferred no TBN"); // "normal debug" "deferred"

	xengine::Material* mtrPlasma = xengine::MaterialManager::Load(shdPlasma);
	mtrPlasma->attribute.bCull = false;
	mtrPlasma->attribute.bBlend = true;
	mtrPlasma->attribute.eBlendSrc = GL_ONE;
	mtrPlasma->attribute.eBlendDst = GL_ONE;
	mtrPlasma->RegisterTexture("TexPerllin", xengine::TextureManager::LoadTexture2D("perlin noise", "textures/perlin.png", GL_RGBA));
	mtrPlasma->RegisterUniform("Strength", 1.5f);
	mtrPlasma->RegisterUniform("Speed", 0.083f);

	// model
	plane_0.InsertMesh(plane, mtrMetal);
	plane_0.SetPosition(glm::vec3(0, -1, 0));
	plane_0.SetScale(glm::vec3(10.0f));

	torus_0.InsertMesh(torus, mtrTorus);
	torus_1.InsertMesh(torus, mtrTorus);
	torus_2.InsertMesh(torus, mtrTorus);

	torus_0.InsertChild(&torus_1);
	torus_1.InsertChild(&torus_2);

	torus_0.SetScale(glm::vec3(1.0f));
	torus_0.SetPosition(glm::vec3(-4.4f, 3.46f, -0.3));
	torus_1.SetScale(glm::vec3(0.65f));
	torus_1.SetRotation(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	torus_2.SetScale(glm::vec3(0.65f));

	plasmaOrb.InsertMesh(sphere, mtrPlasma);
	plasmaOrb.SetPosition(glm::vec3(-4.0f, 4.0f, 0.25f));
	plasmaOrb.SetScale(glm::vec3(0.6f));

	sponza = xengine::ModelManager::LoadFromObj("sponza", "meshes/sponza/sponza.obj");
	sponza->SetPosition(glm::vec3(0.0, -1.0, 0.0));
	sponza->SetScale(glm::vec3(0.01f));

	// light
	dir_light.direction = glm::vec3(0.2f, -1.0f, 0.25f);
	dir_light.color = glm::vec3(1.0f, 0.89f, 0.7f);
	dir_light.intensity = 50.0f;
	dir_light.UpdateShadowView(glm::vec3(0));
	//dir_light.SetRegional(true);

	xengine::PointLight torch;
	torch.radius = 2.5;
	torch.color = glm::vec3(1.0f, 0.3f, 0.05f);
	torch.intensity = 50.0f;
	torch.useVolume = true;

	torchLights.clear();
	torch.position = glm::vec3(4.85f, 0.7f, 1.43f);
	torchLights.push_back(torch);
	torch.position = glm::vec3(4.85f, 0.7f, -2.2f);
	torchLights.push_back(torch);
	torch.position = glm::vec3(-6.19f, 0.7f, 1.43f);
	torchLights.push_back(torch);
	torch.position = glm::vec3(-6.19f, 0.7f, -2.2f);
	torchLights.push_back(torch);

	// environment capture
	xengine::Texture hdrMap = xengine::TextureManager::LoadHDR("sky env", "textures/backgrounds/alley.hdr");

	// allocate image-based lighting renderer
	fbEnvironment = xengine::IblRenderer::CreateEnvironment(hdrMap);
	xengine::CubeMap envMap = fbEnvironment.GetColorAttachment(0);

	fbIrradiance = xengine::IblRenderer::CreateIrradiance(envMap);
	irradianceMap = fbIrradiance.GetColorAttachment(0);

	fbReflection = xengine::IblRenderer::CreateReflection(envMap);
	reflectionMap = fbReflection.GetColorAttachment(0);

	// setup skybox
	skybox.materials[0]->RegisterUniform("lodLevel", 1.5f);
	skybox.SetScale(glm::vec3(1e20f)); // set skybox infinitely big (model size, not the cube)
	skybox.SetCubeMap(envMap);

	// scene
	InsertModel(&plane_0);
	InsertModel(&torus_0);
	InsertModel(&plasmaOrb);
	InsertModel(sponza);
	InsertModel(&skybox);
	AddLight(&dir_light);
	AddLight(&torchLights[0]);
	AddLight(&torchLights[1]);
	AddLight(&torchLights[2]);
	AddLight(&torchLights[3]);
}

void MyScene1::Update(float t, float dt)
{
	torus_0.Rotate(dt * 2.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	torus_1.Rotate(dt * 3.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	torus_2.Rotate(dt * 4.0f, glm::vec3(1.0f, 0.0f, 0.0f));

	plasmaOrb.materials[0]->RegisterUniform("Time", t);

	for (int i = 0; i < torchLights.size(); ++i)
	{
		torchLights[i].radius = 1.8f + 0.3f * std::cosf(std::sinf(t * 1.37f + i * 7.31f) * 3.1f + i);
		torchLights[i].intensity = 50.0f + 15.0f * std::cosf(std::sinf(t * 0.67f + i * 2.31f) * 2.31f * i);
	}
}
