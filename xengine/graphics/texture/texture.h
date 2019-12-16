#pragma once
#ifndef XE_TEXTURE_H
#define XE_TEXTURE_H

namespace xengine
{
	class Texture
	{
	public:
		struct Attribute
		{
			unsigned int target;      // what type of texture
			unsigned int width;       // texture width
			unsigned int height;      // texture height
			unsigned int depth;       // texture depth
			unsigned int colorFormat; // number of color components (internal format)
			unsigned int pixelFormat; // the format each texel is stored (format)
			unsigned int dataType;    // data format
			unsigned int filterMin;   // what filter method to use during minification
			unsigned int filterMax;   // what filter method to use during magnification
			unsigned int wrapS;       // wrapping method of the S coordinate
			unsigned int wrapT;       // wrapping method of the T coordinate
			unsigned int wrapR;       // wrapping method of the R coordinate
			bool mipmapping;

			Attribute();
		};

	public:
		Texture();
		~Texture();

		inline unsigned int ID() const { return m_id; }
		inline unsigned int Width() const { return attribute.width; }
		inline unsigned int Height() const { return attribute.height; }
		inline unsigned int Depth() const { return attribute.depth; }

		// bind texture, activate texture in shaders if unit is given
		void Bind(int unit = -1);

		// unbind texture
		void Unbind();

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
		void SetWrapMode(unsigned int wrapMode);
		void SetFilterMin(unsigned int filter);
		void SetFilterMax(unsigned int filter);

		void EnableMipmap();

		// delete data from memory
		void DeleteGpuData();

	public:
		Attribute attribute;

	private:
		unsigned int m_id = 0;
	};

	using CubeMap = Texture;
}
#endif // !XE_TEXTURE_H
