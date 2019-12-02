#pragma once
#ifndef XE_SHADOW_H
#define XE_SHADOW_H

#include <vendor/glm/glm.hpp>

#include "../frame_buffer/frame_buffer.h"

namespace xengine
{
	class Shadow
	{
	public:
		virtual void Resize(unsigned int width, unsigned int height);

		inline FrameBuffer* GetFrameBuffer() { return &m_shadowMap; }

	protected:
		FrameBuffer m_shadowMap;
	};
}

#endif // !XE_SHADOW_H
