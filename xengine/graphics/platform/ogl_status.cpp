#include "ogl_status.h"

#include <vendor/glad/glad.h>

namespace xengine
{
	bool OglStatus::m_lock = false; // switch lock
	bool OglStatus::m_bDepthTest = GL_TRUE; // ogl toggles
	bool OglStatus::m_bBlend = GL_FALSE;
	bool OglStatus::m_bCull = GL_TRUE;
	unsigned int OglStatus::m_eDepthFunc = GL_LESS; // ogl status
	unsigned int OglStatus::m_eBlendSrc = GL_ONE;
	unsigned int OglStatus::m_eBlendDst = GL_ONE_MINUS_SRC_ALPHA;
	unsigned int OglStatus::m_eBlendEq = GL_FUNC_ADD;
	unsigned int OglStatus::m_eCullFace = GL_BACK;
	unsigned int OglStatus::m_eCullWind = GL_CCW;
	unsigned int OglStatus::m_ePolygonMode = GL_FILL;
	int OglStatus::m_texImgUnitMax = 0; // constants

	void OglStatus::Lock()
	{
		m_lock = true;
	}

	void OglStatus::Unlock()
	{
		m_lock = false;
	}

	void OglStatus::SetDepthTest(bool enable)
	{
		if (m_lock) return;

		if (enable)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
	}

	void OglStatus::SetDepthFunc(unsigned int func)
	{
		if (m_lock) return;

		m_eDepthFunc = func;
		glDepthFunc(func);
	}

	void OglStatus::SetBlend(bool enable)
	{
		if (m_lock) return;

		m_bBlend = enable;

		if (enable)
			glEnable(GL_BLEND);
		else
			glDisable(GL_BLEND);
	}

	void OglStatus::SetBlendFunc(unsigned int src, unsigned int dst)
	{
		if (m_lock) return;

		m_eBlendSrc = src;
		m_eBlendDst = dst;
		glBlendFunc(src, dst);
	}

	void OglStatus::SetCull(bool enable)
	{
		if (m_lock) return;

		m_bCull = enable;

		if (enable)
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);
	}

	void OglStatus::SetCullFace(unsigned int face)
	{
		if (m_lock) return;

		m_eCullFace = face;
		glCullFace(face);
	}

	void OglStatus::SetPolygonMode(unsigned int mode)
	{
		if (m_lock) return;

		m_ePolygonMode = mode;
		glPolygonMode(GL_FRONT_AND_BACK, mode);
	}

	int OglStatus::GetMaxTextureUnit()
	{
		if (m_texImgUnitMax == 0)
			glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &m_texImgUnitMax);

		return m_texImgUnitMax;
	}
}