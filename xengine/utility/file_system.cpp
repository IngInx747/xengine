#include "file_system.h"

#include <filesystem> // c++17
//#include <experimental/filesystem> // c++14

namespace xengine
{
	//using namespace std::experimental; // c++14

	bool FileSystem::Exist(const std::string& path)
	{
		return std::filesystem::exists(path);
	}

	bool FileSystem::IsDirectory(const std::string& directory)
	{
		std::filesystem::path path(directory);
		std::error_code ec;
		return std::filesystem::is_directory(path, ec);
	}
}