#include "xengine.h"

namespace xengine
{
	void xe_initialize(GLFWwindow* & window, unsigned int width, unsigned int height, const std::string& title)
	{
		// initialize system
		Log::Initialize();

		// initialize GLFW
		if (!glfwInit())
		{
			Log::Message("[Init] GLFW initialization failed", Log::ERROR);
			return;
		}
		else
		{
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
			glfwWindowHint(GLFW_RESIZABLE, true);
		}

		// initialize window
		window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

		if (window == NULL)
		{
			Log::Message("[Window] Window creation failed", Log::ERROR);
			return;
		}

		glfwMakeContextCurrent(window);

		// initialize GLAD
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			Log::Message("[Init] GLAD initialization failed", Log::ERROR);
			return;
		}
		else
		{
			Log::Message("[Init] Version - Major: " + std::to_string(GLVersion.major) + " Minor: " + std::to_string(GLVersion.minor), Log::INFO);
			Log::Message("[Init] Driver: " + std::string((char*)glGetString(GL_VENDOR)) + " Renderer: " + std::string((char*)glGetString(GL_RENDERER)), Log::INFO);
		}

		// Pre-filter convolution artifacts
		// https://learnopengl.com/PBR/IBL/Specular-IBL
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

		// default resources
		ShaderManager::Initialize();
		TextureManager::Initialize();
		MaterialManager::Initialize();
		MeshManager::Initialize();
		ModelManager::Initialize();

		// ui
		UI::Initialize(window);

		// generate uniform buffers
		Renderer::Initialize();
	}

	void xe_terminate()
	{
		UI::Clear();
		ModelManager::Clear();
		MeshManager::Clear();
		MaterialManager::Clear();
		TextureManager::Clear();
		ShaderManager::Clear();

		glfwTerminate();
	}
}