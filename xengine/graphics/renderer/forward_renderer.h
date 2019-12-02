#ifndef XE_FORWARD_RENDERER_H
#define XE_FORWARD_RENDERER_H

#include <vector>

#include <camera/camera.h>

#include "../light/parallel_light.h"
#include "../light/point_light.h"
#include "../command/render_command.h"

namespace xengine
{
	class ForwardRenderer
	{
	public:
		ForwardRenderer();

		// generate shadow maps of all parallel lights given a scene (shadow-cast commands)
		void GenerateShadowParallelLights(const std::vector<RenderCommand>& commands, const std::vector<ParallelLight*>& lights, Camera* camera);

		// render emissive sphere of point lights
		void RenderEmissionPointLights(const std::vector<PointLight*>& lights, Camera* camera, float radius = -1.0f);

	public:
		// set shadow maps of all parallel lights to FORWARD commands
		static void SetShadowParallelLights(const std::vector<ParallelLight*>& lights, const std::vector<RenderCommand>& commands);

		// render a scene (forward commands)
		static void RenderForwardCommands(const std::vector<RenderCommand>& commands);

	private:
		Shader* m_parallelShadowShader;
		Shader* m_volumnLightShader;

		Mesh* m_sphere;
	};
}

#endif // !XE_FORWARD_RENDERER_H
