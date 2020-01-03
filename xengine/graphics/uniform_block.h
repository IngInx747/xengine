#pragma once
#ifndef XE_UNIFORM_BLOCK_H
#define XE_UNIFORM_BLOCK_H

#include "uniform_buffer.h"

namespace xengine
{
	class UniformBlock
	{
	public:
		void Register(UniformBuffer* ub);

		void SetBlock(unsigned int offset, unsigned int size);

		// clear runtime status before committing data
		void Refresh();

		// push data into uniform block
		// data should be registered in the order they are defined in shaders
		void CommitData(const glm::vec3& data);
		void CommitData(const glm::vec4& data);
		void CommitData(const glm::mat4& data);

	private:
		// customer
		UniformBuffer* m_ub;

		// block attributes
		unsigned int m_offset;
		unsigned int m_size;

		// runtime
		unsigned int m_position; // current offset
	};
}

#endif // !XE_UNIFORM_BLOCK_H
