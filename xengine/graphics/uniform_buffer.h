#pragma once
#ifndef XE_UNIFORM_BUFFER_H
#define XE_UNIFORM_BUFFER_H

#include <vector>

#include <vendor/glm/glm.hpp>

namespace xengine
{
	// Uniform Block Layout: std 140
	class UniformBuffer
	{
	public:
		UniformBuffer();

		unsigned int ID();
		unsigned int BindPoint();

		void Bind();
		void Unbind();

		// allocate memory and set binding point
		void Generate(unsigned int size, unsigned int bp);

		// send data to uniform block
		void Commit(void* data, unsigned int offset, unsigned int size);

	private:
		unsigned int m_id = 0; // ubo (uniform buffer object)
		unsigned int m_bp = 0; // binding point
	};
}

#endif // !XE_UNIFORM_BUFFER_H
