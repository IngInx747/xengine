#include "smart_handle.h"

namespace xengine
{
	////////////////////////////////////////////////////////////////
	// Shared Memory
	////////////////////////////////////////////////////////////////

	SharedMemory::SharedMemory()
	{
		IncreaseReference();
	}

	SharedMemory::~SharedMemory()
	{
		Destory();
	}

	void SharedMemory::IncreaseReference()
	{
		m_counter.Increment();
	}

	void SharedMemory::DecreaseReference()
	{
		m_counter.Decrement();
	}

	unsigned long long SharedMemory::ReferenceCount()
	{
		return m_counter.Value();
	}

	void SharedMemory::Generate()
	{
	}

	void SharedMemory::Destory()
	{
	}

	////////////////////////////////////////////////////////////////
	// Shared Handle
	////////////////////////////////////////////////////////////////
}