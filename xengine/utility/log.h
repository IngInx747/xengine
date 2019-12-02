#pragma once
#ifndef XE_LOG_H
#define XE_LOG_H

#include <string>

namespace xengine
{
	////////////////////////////////////////////////////////////////
	// Log
	////////////////////////////////////////////////////////////////

	class Log
	{
	public:
		enum TYPE
		{
			INFO = 0x01,
			DEBUG = 0x02,
			WARN = 0x04,
			ERROR = 0x08,
		};

	public:
		static void Initialize();

		// print message of type to outstream
		static void Message(const std::string& message, unsigned int type, std::ostream& os);

		// print message of type to standard outstream
		static void Message(const std::string& message, unsigned int type);

		// enable printing certain type of messages
		static void Enable(unsigned int flag);

		// disable printing certain type of messages
		static void Disable(unsigned int flag);

	private:
		// filter to hide certain type(s) of message
		static unsigned int _filter;

		// check whether console supports colored font
		static bool _enableColorFont;
	};
}

#endif // !XE_LOG_H
