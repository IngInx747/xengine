#include "xengine.h"

namespace xengine
{
	// OpenGL debug
	void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, void *userParam);

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

		// initialize OpenGL debug output
#ifdef _DEBUG
		int context_flags;
		glGetIntegerv(GL_CONTEXT_FLAGS, &context_flags);

		if (context_flags & GL_CONTEXT_FLAG_DEBUG_BIT)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback((GLDEBUGPROC)glDebugOutput, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);
			Log::Message("[Init] Debug output enabled", Log::DEBUG);
		}
		else
		{
			Log::Message("[Init] Debug output not supported", Log::WARN);
		}
#endif // DEBUG

		// skybox convolution artifacts
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

		// image-based lighting
		IblRenderer::Initialize();

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

	void APIENTRY glDebugOutput(
		GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar *message,
		void *userParam)
	{
		// ignore non-significant error/warning codes
		if (
			id == 131169 || id == 131185 || id == 131218 || id == 131204 || id || // driver-specific non-significant error codes
			id == 2000 || id == 2001 || id == 2265 // shader compilation error codes; ignore as already managed from shader object
			)
		{ return; }

		std::string logMessage = "Debug output: (" + std::to_string(id) + "): " + message + "\n";

		switch (source)
		{
		case GL_DEBUG_SOURCE_API:             logMessage += "Source: API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   logMessage += "Source: Window System"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: logMessage += "Source: Shader Compiler"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:     logMessage += "Source: Third Party"; break;
		case GL_DEBUG_SOURCE_APPLICATION:     logMessage += "Source: Application"; break;
		case GL_DEBUG_SOURCE_OTHER:           logMessage += "Source: Other"; break;
		} logMessage += "\n";

		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:               logMessage += "Type: Error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: logMessage += "Type: Deprecated Behaviour"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  logMessage += "Type: Undefined Behaviour"; break;
		case GL_DEBUG_TYPE_PORTABILITY:         logMessage += "Type: Portability"; break;
		case GL_DEBUG_TYPE_PERFORMANCE:         logMessage += "Type: Performance"; break;
		case GL_DEBUG_TYPE_MARKER:              logMessage += "Type: Marker"; break;
		case GL_DEBUG_TYPE_PUSH_GROUP:          logMessage += "Type: Push Group"; break;
		case GL_DEBUG_TYPE_POP_GROUP:           logMessage += "Type: Pop Group"; break;
		case GL_DEBUG_TYPE_OTHER:               logMessage += "Type: Other"; break;
		} logMessage += "\n";

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         logMessage += "Severity: high"; break;
		case GL_DEBUG_SEVERITY_MEDIUM:       logMessage += "Severity: medium"; break;
		case GL_DEBUG_SEVERITY_LOW:          logMessage += "Severity: low"; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: logMessage += "Severity: notification"; break;
		} logMessage += "\n";
		logMessage += "\n";

		// only log a message a maximum of 3 times (as it'll keep spamming the message queue with
		// the same error message)
		static unsigned int msgCount = 0;
		if (msgCount++ < 3)
		{
			Log::Message(logMessage, type == GL_DEBUG_TYPE_ERROR ? Log::ERROR : Log::WARN);
		}
	}
}