#pragma once
#ifndef XE_TEXTURE_H
#define XE_TEXTURE_H

#include <utility/smart_handle.h>

namespace xengine
{
	class TextureMemory : public SharedMemory
	{
	public:
		TextureMemory();

		void Generate(); // allocate memory on GPU
		void Destory(); // free memory on GPU

	public:
		unsigned int m_id = 0;    // texture object
		unsigned int target;      // what type of texture
		unsigned int width = 0;   // texture width
		unsigned int height = 0;  // texture height
		unsigned int depth = 0;   // texture depth
		unsigned int colorFormat; // number of color components (internal format)
		unsigned int pixelFormat; // the format each texel is stored (format)
		unsigned int dataType;    // data format
		unsigned int filterMin;   // what filter method to use during minification
		unsigned int filterMax;   // what filter method to use during magnification
		unsigned int wrapS;       // wrapping method of the S coordinate
		unsigned int wrapT;       // wrapping method of the T coordinate
		unsigned int wrapR;       // wrapping method of the R coordinate
		bool mipmapping;
	};

	class Texture : public SharedHandle<TextureMemory>
	{
	public:
		// bind texture, activate texture in shaders if unit is given
		void Bind(int unit = -1);
		void Bind(int unit = -1) const;

		// unbind texture
		void Unbind() const;

		// resize texture, re-allocate memory
		void Resize(unsigned int width, unsigned int height = 0, unsigned int depth = 0);

		// generate a 1D texture, allocate memory
		void Generate1D(unsigned int width, unsigned int colorFormat, unsigned int pixelFormat, unsigned int data_type, void* data);

		// generate a 2D texture, allocate memory
		void Generate2D(unsigned int width, unsigned int height, unsigned int colorFormat, unsigned int pixelFormat, unsigned int data_type, void* data);

		// generate a 3D texture, allocate memory
		void Generate3D(unsigned int width, unsigned int height, unsigned int depth, unsigned int colorFormat, unsigned int pixelFormat, unsigned int data_type, void* data);

		// generate a cubic texture, allocate memory
		void GenerateCube(unsigned int width, unsigned int height, unsigned int format, unsigned int data_type, bool mipmap);

		// generate single face of a cubic texture, allocate memory
		void GenerateCube(unsigned int width, unsigned int height, unsigned int format, unsigned int data_type, unsigned int face, unsigned char* data);

		// update relevant texture state
		void SetFilterMin(unsigned int filter);
		void SetFilterMax(unsigned int filter);
		void SetWrapSTR(unsigned int wrapMode);
		void SetWrapS(unsigned int wrapMode);
		void SetWrapT(unsigned int wrapMode);
		void SetWrapR(unsigned int wrapMode);
		void SetMipmap(bool mipmap);

		explicit operator bool() const { return m_ptr && m_ptr->m_id; }

		inline unsigned int ID() const { return m_ptr->m_id; }
		inline unsigned int Target() const { return m_ptr->target; }
		inline unsigned int Width() const { return m_ptr->width; }
		inline unsigned int Height() const { return m_ptr->height; }
		inline unsigned int Depth() const { return m_ptr->depth; }
		inline unsigned int FilterMin() const { return m_ptr->filterMin; }
		inline unsigned int FilterMax() const { return m_ptr->filterMax; }
		inline unsigned int WrapS() const { return m_ptr->wrapS; }
		inline unsigned int WrapT() const { return m_ptr->wrapT; }
		inline unsigned int WrapR() const { return m_ptr->wrapR; }
		inline bool Mipmap() const { return m_ptr->mipmapping; }
	};

	using CubeMap = Texture;
}
#endif // !XE_TEXTURE_H
