#pragma once
#ifndef XE_IBL_RENDERER_H
#define XE_IBL_RENDERER_H

#include <vector>
#include <memory>

#include <vendor/glm/common.hpp>

#include <camera/camera.h>
#include <model/model.h>

#include "../command/render_command.h"
#include "../capture/cubic_capture.h"
#include "general_renderer.h"

namespace xengine
{
	// Image based renderer
	class IblRenderer
	{
	public:
		IblRenderer();

		// generate all captures given an HDR environment texture
		void GenerateEnvironment(Texture* environment);

		// generate environment cube map from given 2D HDR environment texture
		void GenerateEnvCubeMap(Texture* environment);

		// generate irradiance cube map from captured environment cube map
		void GenerateIrradiance(CubeMap* capture);

		// generate reflection cube map from captured environment cube map
		void GenerateReflection(CubeMap* capture);

		CubeMap* GetEnvironment() { return m_environmentCubeMap; }
		CubeMap* GetIrradiance() { return m_envIrradianceCubeMap; }
		CubeMap* GetReflection() { return m_envReflectionCubeMap; }

	public:
		static Texture* GetBrdfIntegrationMap() { return _brdfIntegrationMap; }

	private:
		static void generateNormalRoughnessLookup();

	private:
		// captures
		CubicCapture m_environmentCapture; // environment HDR texture equirectangular capture
		CubicCapture m_irradianceCapture; // environment irradiance capture
		CubicCapture m_reflectionCapture; // environment reflection capture

		// related frame buffers

		// captures
		CubeMap* m_environmentCubeMap; // environment cubemap
		CubeMap* m_envIrradianceCubeMap; // environment irradiance cubemap
		CubeMap* m_envReflectionCubeMap; // environment reflection cubemap

		// related shaders
		Shader* m_environmentCaptureShader; // convert HDR environment 2D texture to environment cubemap
		Shader* m_irradianceCaptureShader; // generate the irradiance cubemap from environment cubemap
		Shader* m_reflectionCaptureShader; // generate the reflection cubemap from environment cubemap

		// related meshes
		Mesh* m_quad;
		Mesh* m_cube;
		Mesh* m_sphere;

	private:
		static Texture* _brdfIntegrationMap; // pre-calculated normal-roughness lookup texture
		static FrameBuffer _brdfIntegrationMapBuffer; // pre-calculated normal-roughness lookup frame buffer
	};
}

#endif // !XE_IBL_RENDERER_H
