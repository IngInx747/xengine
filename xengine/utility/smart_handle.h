#pragma once
#ifndef XE_SMART_HANDLE_H
#define XE_SMART_HANDLE_H

#include <type_traits>

#include "counter.h"

namespace xengine
{
	class SharedMemory
	{
	public:
		SharedMemory();
		virtual ~SharedMemory();

		// Note: As the struct holds unique resource, instance copy is not allowed
		SharedMemory(const SharedMemory& other) = delete;
		SharedMemory & operator=(const SharedMemory& other) = delete;

		void IncreaseReference();
		void DecreaseReference();
		unsigned long long ReferenceCount();

		// allocate memory for resource
		virtual void Generate();

		// free memory of resource
		virtual void Destory();

	protected:
		Counter m_counter;
	};

	template <class T>
	class SharedHandle
	{
		// To use features of SharedMemory, the class T must be derived class of base class SharedMemory
		static_assert(std::is_base_of<SharedMemory, T>::value, "SharedHandle<T> Class T must derive from SharedMemory");

	public:
		SharedHandle()
		{
		}

		virtual ~SharedHandle()
		{
			if (m_ptr)
			{
				m_ptr->DecreaseReference();

				if (m_ptr->ReferenceCount() == 0)
				{
					m_ptr->Destory();
					delete m_ptr;
				}

				m_ptr = nullptr;
			}
		}

		SharedHandle(const SharedHandle& other)
		{
			m_ptr = other.m_ptr;
			if (m_ptr) m_ptr->IncreaseReference();
		}

		SharedHandle & operator=(const SharedHandle& other)
		{
			if (m_ptr)
			{
				m_ptr->DecreaseReference();

				if (m_ptr->ReferenceCount() == 0)
				{
					m_ptr->Destory();
					delete m_ptr;
				}
			}

			m_ptr = other.m_ptr;
			if (m_ptr) m_ptr->IncreaseReference();

			return *this;
		}

	protected:
		// allocate handle memory on the platform (like CPU)
		virtual void allocateMemory()
		{
			if (m_ptr) return;

			m_ptr = new T;
		}

		// allocate resource on the device (like GPU)
		virtual void generateObject()
		{
			m_ptr->Generate();
		}

		// allocate memory and resource
		virtual void generate()
		{
			allocateMemory();
			generateObject();
		}

	protected:
		// the pointer of SharedMemory object
		T* m_ptr = nullptr;
	};
}

#endif // !XE_SMART_HANDLE_H
