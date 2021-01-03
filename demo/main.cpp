#include <xengine.h>
#include "scenes.h"

void glfwFramebufferSizeCallback(GLFWwindow* window, int width, int height);
void glfwKeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void glfwMouseMoveCallback(GLFWwindow* window, double xpos, double ypos);
void glfwMousePressCallback(GLFWwindow* window, int button, int action, int mods);
void glfwMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void processMouseMove(float deltaTime);

GLFWwindow* mainWindow;
xengine::EularCamera* mainCamera;
xengine::Renderer* mainRenderer;
const int ScreenWidth = 1600;
const int ScreenHeight = 900;
bool keysPressed[1024];
bool keysActive[1024];
int sceneId = 3;

int main(int argc, char** argv)
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// OpenGL & Engine initialization
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	xengine::xe_initialize(mainWindow, ScreenWidth, ScreenHeight, "XEngine Demo");

	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// register callbacks
	glfwSetFramebufferSizeCallback(mainWindow, glfwFramebufferSizeCallback);
	glfwSetKeyCallback(mainWindow, glfwKeyboardCallback);
	glfwSetCursorPosCallback(mainWindow, glfwMouseMoveCallback);
	glfwSetMouseButtonCallback(mainWindow, glfwMousePressCallback);
	glfwSetScrollCallback(mainWindow, glfwMouseScrollCallback);

	// set main camera
	auto sptrCamera = std::make_shared<xengine::EularCamera>(glm::vec3(0, 1, 0));
	mainCamera = sptrCamera.get();
	mainCamera->SetProjPerspective(glm::radians(60.0f), (float)ScreenWidth / (float)ScreenHeight, 0.1f, 100.0f);
	
	// primary renderer
	auto sptrRenderer = std::make_shared<xengine::Renderer>();
	mainRenderer = sptrRenderer.get();

	mainRenderer->Resize(ScreenWidth, ScreenHeight);

	//printf("Shd %zd\n",sizeof(xengine::Shader));
	//printf("TEX %zd\n",sizeof(xengine::Texture));
	//printf("MTR %zd\n",sizeof(xengine::Material));
	//printf("FBF %zd\n",sizeof(xengine::FrameBuffer));
	//printf("MSH %zd\n",sizeof(xengine::Mesh));
	//printf("MOD %zd\n",sizeof(xengine::Model));

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Main loop
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	while (!glfwWindowShouldClose(mainWindow))
	{
		std::shared_ptr<xengine::Scene> scene;

		if (sceneId == 1)
		{
			scene = std::make_shared<MyScene1>();
			MyScene1* myScene = dynamic_cast<MyScene1*>(scene.get());
			scene->Initialize();
		}
		else if (sceneId == 2)
		{
			scene = std::make_shared<MyScene2>();
			MyScene2* myScene = dynamic_cast<MyScene2*>(scene.get());
			scene->Initialize();
		}
		else if (sceneId == 3)
		{
			scene = std::make_shared<xengine::Scene>();
		}

		if (!scene) break;
		sceneId = 0;

		while (!glfwWindowShouldClose(mainWindow))
		{
			// load a new scene if signal is received
			if (sceneId > 0) break;

			glfwPollEvents();

			xengine::UI::OnRender();

			static float lastFrameTime = 0.0f;
			float currentFrameTime = (float)glfwGetTime();
			float deltaTime = currentFrameTime - lastFrameTime;
			lastFrameTime = currentFrameTime;

			// capture mouse actions and update primary camera
			processMouseMove(deltaTime);
			mainCamera->Update(deltaTime);

			// move objects and update lights in the scene
			scene->Update(currentFrameTime, deltaTime);

			// primary render pipeline
			mainRenderer->Render(scene.get(), mainCamera, xengine::FrameBuffer());

			// ui
			xengine::UI::Render();
			if (xengine::UI::IsVisible()) glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			else glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

			glfwSwapBuffers(mainWindow);
		}

		scene->Clear();
	}

	xengine::xe_terminate();
}

void glfwFramebufferSizeCallback(GLFWwindow *window, int width, int height)
{
	//mainCamera->SetProjPerspective(glm::radians(60.0f), (float)width / (float)height, 0.1f, 100.0f);
	mainRenderer->Resize(width, height);
	glViewport(0, 0, width, height);
}

void glfwKeyboardCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(mainWindow, true);
	}

	// if 1/2/3 is pressed, scene will be reloaded (to test if memory keeps increasing or not)
	if (key == GLFW_KEY_1 && action == GLFW_PRESS) { sceneId = 1; }
	if (key == GLFW_KEY_2 && action == GLFW_PRESS) { sceneId = 2; }
	if (key == GLFW_KEY_3 && action == GLFW_PRESS) { sceneId = 3; }

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keysPressed[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keysPressed[key] = false;
			keysActive[key] = false;
		}
	}
}

void processMouseMove(float deltaTime)
{
	if (keysPressed[GLFW_KEY_W] || keysPressed[GLFW_KEY_UP])
	{
		mainCamera->OpMove(deltaTime, xengine::EularCamera::FORWARD);
	}
	if (keysPressed[GLFW_KEY_S] || keysPressed[GLFW_KEY_DOWN])
	{
		mainCamera->OpMove(deltaTime, xengine::EularCamera::BACK);
	}
	if (keysPressed[GLFW_KEY_A] || keysPressed[GLFW_KEY_LEFT])
	{
		mainCamera->OpMove(deltaTime, xengine::EularCamera::LEFT);
	}
	if (keysPressed[GLFW_KEY_D] || keysPressed[GLFW_KEY_RIGHT])
	{
		mainCamera->OpMove(deltaTime, xengine::EularCamera::RIGHT);
	}
	if (keysPressed[GLFW_KEY_E])
	{
		mainCamera->OpMove(deltaTime, xengine::EularCamera::UP);
	}
	if (keysPressed[GLFW_KEY_Q])
	{
		mainCamera->OpMove(deltaTime, xengine::EularCamera::DOWN);
	}
	if (keysPressed[GLFW_KEY_TAB] && !keysActive[GLFW_KEY_TAB])
	{
		xengine::UI::FlipSwitch();
		keysActive[GLFW_KEY_TAB] = true;
	}
}

void glfwMouseMoveCallback(GLFWwindow *window, double xpos, double ypos)
{
	static bool firstMouse = true;
	static double lastX = ScreenWidth / 2.f;
	static double lastY = ScreenHeight / 2.f;

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	if (!xengine::UI::IsVisible())
	{
		mainCamera->OpView((float)xoffset, (float)yoffset);
	}
}

void glfwMousePressCallback(GLFWwindow*, int button, int action, int mods)
{
}

void glfwMouseScrollCallback(GLFWwindow*, double xoffset, double yoffset)
{
	mainCamera->OpZoom((float)xoffset, (float)yoffset);
}