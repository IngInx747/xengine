#include "file_system.h"

//#include <filesystem> // c++17
#include <experimental/filesystem> // c++14

namespace xengine
{
	using namespace std::experimental;

	bool FileSystem::Exist(const std::string& path)
	{
		return filesystem::exists(path);
	}

	bool FileSystem::IsDirectory(const std::string& directory)
	{
		filesystem::path path(directory);
		std::error_code ec;
		return filesystem::is_directory(path, ec);
	}
}