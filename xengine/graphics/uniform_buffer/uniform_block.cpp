#include "uniform_block.h"

namespace xengine
{
	void UniformBlock::Register(UniformBuffer * ub)
	{
		m_ub = ub;
	}

	void UniformBlock::SetBlock(unsigned int offset, unsigned int size)
	{
		m_offset = offset;
		m_size = size;
		m_position = offset;
	}

	void UniformBlock::Refresh()
	{
		m_position = m_offset;
	}

	void UniformBlock::CommitData(const glm::vec3& data)
	{
		unsigned int size = sizeof(glm::vec3);
		unsigned int stride = sizeof(glm::vec4);
		m_ub->Commit((void*)&data[0], m_position, size);
		m_position += stride;
	}

	void UniformBlock::CommitData(const glm::vec4& data)
	{
		unsigned int size = sizeof(glm::vec4);
		unsigned int stride = sizeof(glm::vec4);
		m_ub->Commit((void*)&data[0], m_position, size);
		m_position += stride;
	}

	void UniformBlock::CommitData(const glm::mat4& data)
	{
		unsigned int size = sizeof(glm::mat4);
		unsigned int stride = sizeof(glm::mat4);
		m_ub->Commit((void*)&data[0][0], m_position, size);
		m_position += stride;
	}
}
