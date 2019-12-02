#pragma once
#ifndef XE_TIME_H
#define XE_TIME_H

#include <string>
#include <cstdint>

namespace xengine
{
	class Timer
	{
	public:
		Timer(const std::string& name);
		~Timer();

	private:
		std::string m_name;
		std::uint64_t m_start;

	private:
		static std::uint64_t s_frequency;
	};
}

#endif // !XE_TIME_H
