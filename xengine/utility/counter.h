#pragma once
#ifndef XE_COUNTER_H
#define XE_COUNTER_H

#include <atomic>

namespace xengine
{
	class Counter
	{
	public:
		Counter();

		void Reset();
		unsigned long long Value();
		unsigned long long Increment();

	private:
		std::atomic<unsigned long long> m_count;
	};
}

#endif // !XE_COUNTER_H
