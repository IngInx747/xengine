#include "parallel_light.h"

#include <vendor/glad/glad.h>
#include <vendor/glm/glm.hpp>
#include <vendor/glm/gtc/matrix_transform.hpp>

#include <geometry/constant.h>

namespace xengine
{
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
}