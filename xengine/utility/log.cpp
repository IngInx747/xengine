#include "log.h"

#include <iostream>

#ifdef _WIN32
#include <windows.h>

typedef NTSTATUS(WINAPI*RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

// Check if STD_OUTPUT_HANDLE or STD_ERROR_HANDLE is redirected to a file.
BOOL IsRedirectedToFile(DWORD stdHandle) {
	BOOL result = FALSE;

	HANDLE hStd = GetStdHandle(stdHandle);
	if (hStd != INVALID_HANDLE_VALUE) {
		if (GetFinalPathNameByHandle(hStd, NULL, 0, 0) != 0 || GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
			result = TRUE;
		}
	}

	return result;
}

// Check if the current console supports ANSI colors.
BOOL HaveColorSupport() {
	static BOOL result = 2;
	if (result == 2) {
		const DWORD MINV_MAJOR = 10, MINV_MINOR = 0, MINV_BUILD = 10586;
		result = FALSE;
		HMODULE hMod = GetModuleHandle(TEXT("ntdll.dll"));
		if (hMod) {
			RtlGetVersionPtr fn = (RtlGetVersionPtr)GetProcAddress(hMod, "RtlGetVersion");
			if (fn != NULL) {
				RTL_OSVERSIONINFOW rovi = { 0 };
				rovi.dwOSVersionInfoSize = sizeof(rovi);
				if (fn(&rovi) == 0) {
					if (
						rovi.dwMajorVersion > MINV_MAJOR
						||
						(
							rovi.dwMajorVersion == MINV_MAJOR
							&&
							(
								rovi.dwMinorVersion > MINV_MINOR
								||
								(
									rovi.dwMinorVersion == MINV_MINOR
									&& rovi.dwBuildNumber >= MINV_BUILD
									)
								)
							)
						) {
						result = TRUE;
					}
				}
			}
		}
	}
	return result;
}

// Check if the current console has ANSI colors enabled.
BOOL ColorSupportEnabled() {
	BOOL result = FALSE;
	if (HaveColorSupport()) {
		HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
		if (hStdOut != INVALID_HANDLE_VALUE) {
			DWORD mode;
			if (GetConsoleMode(hStdOut, &mode)) {
				if (mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) {
					result = TRUE;
				}
			}
		}
	}

	return result;
}

// Enable/disable ANSI colors support for the current console.
// Returns TRUE if operation succeeded, FALSE otherwise.
BOOL EnableColorSupport(BOOL enabled)
{
	BOOL result = FALSE;
	if (HaveColorSupport()) {
		HANDLE hStdOut;
		hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
		if (hStdOut != INVALID_HANDLE_VALUE) {
			DWORD mode;
			if (GetConsoleMode(hStdOut, &mode)) {
				if (((mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) ? 1 : 0) == (enabled ? 1 : 0)) {
					result = TRUE;
				}
				else {
					if (enabled) {
						mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
					}
					else {
						mode &= ~ENABLE_VIRTUAL_TERMINAL_PROCESSING;
					}
					if (SetConsoleMode(hStdOut, mode)) {
						result = TRUE;
					}
				}
			}
		}
	}

	return result;
}
#else
int is_redirected_to_file()
{
	if (!isatty(fileno(stdout))) return 1;
	return 0;
}
#endif

namespace xengine
{
	////////////////////////////////////////////////////////////////
	// Log
	////////////////////////////////////////////////////////////////

#ifdef _WIN32
#pragma push_macro("ERROR")
#undef ERROR
#endif

	// filter is initialized not to hide any types of message
	unsigned int Log::_filter = Log::INFO | Log::DEBUG | Log::WARN | Log::ERROR;

	// check whether console supports colored font
	bool Log::_enableColorFont = false;

	void Log::Initialize()
	{
#ifdef _WIN32
		_enableColorFont = EnableColorSupport(TRUE);
#else
		_enableColorFont = true;
#endif
	}

	void Log::Message(const std::string & message, unsigned int type, std::ostream & os)
	{
		if ((type & _filter) == 0) return;

		if (&os == &std::cout) return Message(message, type);

		std::string header;

		if (type == Log::INFO)       header = "[XE][INFO]";
		else if (type == Log::DEBUG) header = "[XE][DEBUG]";
		else if (type == Log::WARN)  header = "[XE][WARN]";
		else if (type == Log::ERROR) header = "[XE][ERROR]";

		os << header << message << std::endl;
	}

	void Log::Message(const std::string & message, unsigned int type)
	{
		if ((type & _filter) == 0) return;

		std::string header;

#ifdef _WIN32
		if (_enableColorFont && !IsRedirectedToFile(STD_OUTPUT_HANDLE))
		{
			if (type == Log::INFO)       header = "[XE][\x1b[92mINFO\x1b[0m]";
			else if (type == Log::DEBUG) header = "[XE][\x1b[94mDEBUG\x1b[0m]";
			else if (type == Log::WARN)  header = "[XE][\x1b[93mWARN\x1b[0m]";
			else if (type == Log::ERROR) header = "[XE][\x1b[91mERROR\x1b[0m]";
		}
		else
		{
			if (type == Log::INFO)       header = "[XE][INFO]";
			else if (type == Log::DEBUG) header = "[XE][DEBUG]";
			else if (type == Log::WARN)  header = "[XE][WARN]";
			else if (type == Log::ERROR) header = "[XE][ERROR]";
		}
#else
		if (_enableColorFont && !is_redirected_to_file())
		{
			if (type == Log::INFO)       header = "[XE][\033[1;37INFO\033[0m]";
			else if (type == Log::DEBUG) header = "[XE][\033[1;34DEBUG\033[0m]";
			else if (type == Log::WARN)  header = "[XE][\033[1;33WARN\033[0m]";
			else if (type == Log::ERROR) header = "[XE][\033[1;31ERROR\033[0m]";
		}
		else
		{
			if (type == Log::INFO)       header = "[XE][INFO]";
			else if (type == Log::DEBUG) header = "[XE][DEBUG]";
			else if (type == Log::WARN)  header = "[XE][WARN]";
			else if (type == Log::ERROR) header = "[XE][ERROR]";
		}
#endif

		std::cout << header << message << std::endl;
	}

	void Log::Enable(unsigned int flag)
	{
		_filter |= flag;
	}

	void Log::Disable(unsigned int flag)
	{
		_filter &= ~flag;
	}

#ifdef _WIN32
#pragma pop_macro("ERROR")
#endif
}