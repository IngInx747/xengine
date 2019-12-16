#include "counter.h"

namespace xengine
{
	Counter::Counter()
	{
		m_count = 0;
	}

	void Counter::Reset()
	{
		m_count = 0;
	}

	unsigned long long Counter::Value()
	{
		return m_count;
	}

	unsigned long long Counter::Increment()
	{
		return std::atomic_fetch_add(&m_count, 1);
	}

	unsigned long long Counter::Decrement()
	{
		if (m_count == 0) return 0;
		return std::atomic_fetch_sub(&m_count, 1);
	}
}