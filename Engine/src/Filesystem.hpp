#pragma once

#include <string>
#include <physfs.h>

#include "Logger.hpp"

namespace Engine {
	namespace Filesystem {
		 void Init(const char* argv0);
		 void Destroy();

		 void AddToSearchPath(std::string path);

		 bool FileExists(std::string path);
		 int FileSize(std::string path);

		 std::string GetFileContentString(std::string path);
	}
}