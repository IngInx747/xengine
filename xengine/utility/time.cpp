#include "time.h"

#include <iostream>
#include <intrin.h>

namespace xengine
{
	std::uint64_t Timer::s_frequency = 3600000000;

	Timer::Timer(const std::string& name)
		:
		m_name(name),
		m_start(__rdtsc())
	{
	}

	Timer::~Timer()
	{
		std::uint64_t end = __rdtsc();
		std::uint64_t interval = end - m_start;
		float clockTime = static_cast<float>(interval) / static_cast<float>(s_frequency) * 1000.0f;
		std::cout << m_name << ":  " << interval << " cycles | " << clockTime << " ms" << std::endl;
	}
}