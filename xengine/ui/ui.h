#pragma once
#ifndef XE_UI_H
#define XE_UI_H

#include <vendor/imgui/imgui.h>
#include <vendor/imgui/imgui_impl_glfw.h>
#include <vendor/imgui/imgui_impl_opengl3.h>

struct GLFWwindow;

namespace xengine
{
	class UI
	{
	public:
		static void Initialize(GLFWwindow* window);
		static void Clear();
		static void OnRender();
		static void Render();

		static bool IsVisible() { return _visible; }
		static void Enable() { _visible = true; }
		static void Disable() { _visible = false; }
		static void FlipSwitch() { _visible = !_visible; }

	private:
		static bool _visible;
	};
}

#endif // !XE_UI_H
