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
		m_memPtr = other.m_memPtr;
		m_memPtr->IncreaseReference();
	}

	SharedHandle & SharedHandle::operator=(const SharedHandle & other)
	{
		if (m_memPtr)
		{
			m_memPtr->DecreaseReference();

			if (m_memPtr->ReferenceCount() == 0)
			{
				delete m_memPtr;
			}
		}

		m_memPtr = other.m_memPtr;
		m_memPtr->IncreaseReference();

		return *this;
	}

	SharedHandle::~SharedHandle()
	{
		if (m_memPtr)
		{
			m_memPtr->DecreaseReference();

			if (m_memPtr->ReferenceCount() == 0)
			{
				delete m_memPtr;
			}

			m_memPtr = nullptr;
		}
	}
}