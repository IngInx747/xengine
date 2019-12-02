#include "ui.h"

#include <vendor/GLFW/glfw3.h>

#include <graphics/renderer/render_config.h>

namespace xengine
{
	bool UI::_visible = true;

	void UI::Initialize(GLFWwindow * window)
	{
		IMGUI_CHECKVERSION();
		ImGui::SetCurrentContext(ImGui::CreateContext());

#if __APPLE__
		// GL 3.2 + GLSL 150
		const char* glsl_version = "#version 150";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
		// GL 3.0 + GLSL 130
		const char* glsl_version = "#version 130";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
		//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);
	}

	void UI::Clear()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void UI::OnRender()
	{
		if (!_visible) return;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void UI::Render()
	{
		if (!_visible) return;

		ImGui::Begin("Renderer", (bool*)1);
		ImGui::Text("FPS %.1f", ImGui::GetIO().Framerate);

		if (ImGui::CollapsingHeader("Lighting Options"))
		{
			ImGui::Checkbox("Irradiance Probe [WIP]", &RenderConfig::_config.useIrradianceGI);
			ImGui::Checkbox("Parallel Shadow", &RenderConfig::_config.useShadows);
			ImGui::Checkbox("Pt Lights Sphere", &RenderConfig::_config.useRenderLights);
		}

		if (ImGui::CollapsingHeader("Effect Options"))
		{
			ImGui::Checkbox("SSAO", &RenderConfig::_config.useSSAO);
			ImGui::Checkbox("Bloom", &RenderConfig::_config.useBloom);
			ImGui::Checkbox("Motion Blur", &RenderConfig::_config.useMotionBlur);
			ImGui::Checkbox("Vignette", &RenderConfig::_config.useVignette);
			ImGui::Checkbox("Sepia", &RenderConfig::_config.useSepia);
		}

		if (ImGui::CollapsingHeader("Debug Options"))
		{
			ImGui::Checkbox("Wireframe", &RenderConfig::_config.useWireframe);
			ImGui::Checkbox("Pt Light Volume", &RenderConfig::_config.useLightVolumes);
			ImGui::Checkbox("Ambient Probes", &RenderConfig::_config.useRenderProbes);
		}

		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		ImGui::EndFrame();
	}
}