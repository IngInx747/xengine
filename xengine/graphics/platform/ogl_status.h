#pragma once
#ifndef XE_OGL_STATUS_H
#define XE_OGL_STATUS_H

#include <stack>

namespace xengine
{
	class OglStatus
	{
	public:
		static void Lock();
		static void Unlock();

		static void SetDepthTest(bool enable);
		static void SetDepthFunc(unsigned int func);
		static void SetBlend(bool enable);
		static void SetBlendFunc(unsigned int src, unsigned int dst);
		static void SetCull(bool enable);
		static void SetCullFace(unsigned int face);
		static void SetPolygonMode(unsigned int mode);

		static int GetMaxTextureUnit();

	private:
		// switch lock
		static bool m_lock;

		// ogl toggles
		static bool m_bDepthTest;
		static bool m_bBlend;
		static bool m_bCull;

		// ogl status
		static unsigned int m_eDepthFunc;
		static unsigned int m_eBlendSrc;
		static unsigned int m_eBlendDst;
		static unsigned int m_eBlendEq;
		static unsigned int m_eCullFace;
		static unsigned int m_eCullWind;
		static unsigned int m_ePolygonMode;

		// constants
		static int m_texImgUnitMax;
	};
}

#endif // !XE_OGL_STATUS_H
