#include "uniform_buffer.h"

#include <glad/glad.h>

namespace xengine
{
	UniformBuffer::UniformBuffer()
	{
	}

	unsigned int xengine::UniformBuffer::ID()
	{
		return m_id;
	}

	unsigned int UniformBuffer::BindPoint()
	{
		return m_bp;
	}

	void UniformBuffer::Bind()
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_id);
	}

	void UniformBuffer::Unbind()
	{
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void UniformBuffer::Generate(unsigned int size, unsigned int bp)
	{
		m_bp = bp;
		if (m_id == 0) glGenBuffers(1, &m_id);
		glBindBuffer(GL_UNIFORM_BUFFER, m_id);
		glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_STREAM_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, m_bp, m_id);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void UniformBuffer::Commit(void * data, unsigned int offset, unsigned int size)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_id);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
	}
}