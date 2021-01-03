#pragma once
#ifndef XE_IBL_RENDERER_H
#define XE_IBL_RENDERER_H

#include <vector>
#include <memory>

#include <glm/common.hpp>

#include <geometry/camera.h>

#include "render_command.h"
#include "cubic_capture.h"
#include "general_renderer.h"

namespace xengine
{
	// Image based renderer
	class IblRenderer
	{
	public:
		static void Initialize();

		// generate environment cube map from given 2D HDR environment texture
		static FrameBuffer CreateEnvironment(const Texture& environment);

		// generate irradiance cube map from captured environment cube map
		static FrameBuffer CreateIrradiance(const CubeMap& environment);

		// generate reflection cube map from captured environment cube map
		static FrameBuffer CreateReflection(const CubeMap& environment);

		static const Texture & GetBrdfIntegrationMap() { return _brdfIntegrationMap; }

	private:
		static void generateNormalRoughnessLookup();

	private:
		// related shaders
		static Shader _environmentCaptureShader; // convert HDR environment 2D texture to environment cubemap
		static Shader _irradianceCaptureShader; // generate the irradiance cubemap from environment cubemap
		static Shader _reflectionCaptureShader; // generate the reflection cubemap from environment cubemap

		// related meshes
		static Mesh _quad;
		static Mesh _cube;
		static Mesh _sphere;

		static Texture _brdfIntegrationMap; // pre-calculated normal-roughness lookup texture
		static FrameBuffer _brdfIntegrationMapBuffer; // pre-calculated normal-roughness lookup frame buffer
	};
}

#endif // !XE_IBL_RENDERER_H
