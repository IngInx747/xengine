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

	////////////////////////////////////////////////////////////////
	// Shared Handle
	////////////////////////////////////////////////////////////////

	SharedHandle::SharedHandle()
	{
	}

	SharedHandle::SharedHandle(const SharedHandle & other)
	{
		m_sharedMemoryPtr = other.m_sharedMemoryPtr;
		m_sharedMemoryPtr->IncreaseReference();
	}

	SharedHandle & SharedHandle::operator=(const SharedHandle & other)
	{
		if (m_sharedMemoryPtr)
		{
			m_sharedMemoryPtr->DecreaseReference();

			if (m_sharedMemoryPtr->ReferenceCount() == 0)
			{
				delete m_sharedMemoryPtr;
			}
		}

		m_sharedMemoryPtr = other.m_sharedMemoryPtr;
		m_sharedMemoryPtr->IncreaseReference();

		return *this;
	}

	SharedHandle::~SharedHandle()
	{
		if (m_sharedMemoryPtr)
		{
			m_sharedMemoryPtr->DecreaseReference();

			if (m_sharedMemoryPtr->ReferenceCount() == 0)
			{
				delete m_sharedMemoryPtr;
			}

			m_sharedMemoryPtr = nullptr;
		}
	}
}