#ifndef XE_FORWARD_RENDERER_H
#define XE_FORWARD_RENDERER_H

#include <vector>

#include <geometry/camera.h>

#include "light.h"
#include "render_command.h"
#include "particle_system.h"

namespace xengine
{
	class ForwardRenderer
	{
	public:
		ForwardRenderer();

		// generate shadow maps of all parallel lights given a scene (shadow-cast commands)
		void GenerateParallelShadow(const std::vector<RenderCommand>& commands, const std::vector<ParallelLight*>& lights, Camera* camera);

		// render emissive sphere of point lights
		void RenderEmissionPointLights(const std::vector<PointLight*>& lights, Camera* camera, float radius = -1.0f);

	public:
		// set shadow maps of all parallel lights to FORWARD commands
		static void SetParallelShadow(const std::vector<ParallelLight*>& lights, const std::vector<RenderCommand>& commands);

		// render a scene (forward commands)
		static void RenderForwardCommands(const std::vector<RenderCommand>& commands);

		//
		static void RenderParticles(const std::vector<ParticleSystem*>& particles, Camera* camera);

	private:
		Shader m_parallelShadowShader;
		Shader m_volumnLightShader;

		Mesh m_sphere;
	};
}

#endif // !XE_FORWARD_RENDERER_H
