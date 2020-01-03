#include "light.h"

namespace xengine
{
	////////////////////////////////////////////////////////////////
	// Light
	////////////////////////////////////////////////////////////////

	Light::Light()
		:
		color(1.0f)
	{}

	Light::Light(const glm::vec3& color)
		:
		color(color)
	{}

	////////////////////////////////////////////////////////////////
	// Light: Parallel Light
	////////////////////////////////////////////////////////////////

	ParallelLight::ParallelLight()
		:
		Light(),
		direction(0.0f, -1.0f, 0.0f),
		intensity(1.0f),
		useShadowCast(true)
	{
		shadow.GenerateShadowMap(2048, 2048);
		shadow.UpdateView(direction, { 0.0f, 0.0f, 0.0f });
	}

	ParallelLight::ParallelLight(const glm::vec3& direction, float intensity)
		:
		Light(),
		direction(direction),
		intensity(intensity),
		useShadowCast(true)
	{
		shadow.GenerateShadowMap(2048, 2048);
		shadow.UpdateView(direction, { 0.0f, 0.0f, 0.0f });
	}

	void ParallelLight::UpdateShadowView(const glm::vec3& center)
	{
		shadow.UpdateView(direction, center);
	}

	void ParallelLight::SetRegional(bool regional)
	{
		shadow.SetBorderDepth(!regional);
	}

	////////////////////////////////////////////////////////////////
	// Light: Point Light
	////////////////////////////////////////////////////////////////

	PointLight::PointLight()
		:
		Light(),
		position(0.0f, 0.0f, 0.0f),
		intensity(1.0f),
		radius(1.0f),
		useVolume(false)
	{}
}