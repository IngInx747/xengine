#pragma once
#ifndef XE_RENDER_CONFIG_H
#define XE_RENDER_CONFIG_H

namespace xengine
{
	class UI;

	class RenderConfig
	{
		friend class UI;

	private:
		struct Config
		{
			bool useIrradianceGI;
			bool useParallelShadow;
			bool useFlashLight;
			bool useRenderLights;
			bool useLightVolumes;
			bool useRenderProbes;
			bool useWireframe;
			bool useSepia;
			bool useVignette;
			bool useBloom;
			bool useSSAO;
			bool useSSR;
			bool useTXAA;
			bool useMotionBlur;

			Config();
		};

	public:
		static bool UseWireframe() { return _config.useWireframe; }
		static bool UseFlashLight() { return _config.useFlashLight; }
		static bool UseLightVolume() { return _config.useLightVolumes; }
		static bool UseRenderLights() { return _config.useRenderLights; }
		static bool UseRenderProbes() { return _config.useRenderProbes; }
		static bool UseParallelShadow() { return _config.useParallelShadow; }
		static bool UseSSAO() { return _config.useSSAO; }
		static bool UseSSR() { return _config.useSSR; }
		static bool UseSepia() { return _config.useSepia; }
		static bool UseVignette() { return _config.useVignette; }
		static bool UseBloom() { return _config.useBloom; }
		static bool UseMotionBlur() { return _config.useMotionBlur; }

	private:
		static Config _config;
	};
}

#endif // !XE_RENDER_CONFIG_H
