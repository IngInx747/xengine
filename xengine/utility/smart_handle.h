#pragma once
#ifndef XE_SMART_HANDLE_H
#define XE_SMART_HANDLE_H

#include "counter.h"

namespace xengine
{
	class SharedMemory
	{
	public:
		SharedMemory();
		virtual ~SharedMemory();

		void IncreaseReference();
		void DecreaseReference();
		unsigned long long ReferenceCount();

	protected:
		Counter m_counter;
	};

	class SharedHandle
	{
	public:
		SharedHandle();
		SharedHandle(const SharedHandle& other);
		SharedHandle & operator=(const SharedHandle& other);
		virtual ~SharedHandle();

	protected:
		// Register resource pointer from derived class
		// The resource class must derive from SharedMemory
		template <class T>
		void Register(T* ptr)
		{
			m_sharedMemoryPtr = dynamic_cast<SharedMemory*>(ptr);
		}

	private:
		SharedMemory* m_sharedMemoryPtr = nullptr;
	};
}

#endif // !XE_SMART_HANDLE_H
