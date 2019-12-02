#include "render_config.h"

namespace xengine
{
	RenderConfig::Config::Config()
	{
		useIrradianceGI = true;
		useShadows = true;
		useFlashLight = true;
		useRenderLights = true;
		useLightVolumes = true;
		useRenderProbes = false;
		useWireframe = false;
		useSepia = false;
		useVignette = true;
		useBloom = true;
		useSSAO = true;
		useTXAA = false;
		useMotionBlur = true;
	}

	RenderConfig::Config RenderConfig::_config;
}