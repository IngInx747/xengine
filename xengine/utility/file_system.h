#pragma once
#ifndef XE_FILE_SYSTEM_H
#define XE_FILE_SYSTEM_H

#include <string>

namespace xengine
{
	class FileSystem
	{
	public:
		static bool Exist(const std::string& path);
		static bool IsDirectory(const std::string& path);
	};
}

#endif // !XE_FILE_SYSTEM_H
