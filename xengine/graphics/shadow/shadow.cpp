#include "shadow.h"

#include <vendor/glad/glad.h>

namespace xengine
{
	void Shadow::Resize(unsigned int width, unsigned int height)
	{
		m_shadowMap.Resize(width, height);
	}
}