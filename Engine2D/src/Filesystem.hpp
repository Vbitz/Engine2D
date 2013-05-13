#pragma once

#include <string>
#include <vector>

#include <physfs.h>

#include "Logger.hpp"

namespace Engine {
	namespace Filesystem {
		bool						IsLoaded();

		void						Init(const char* argv0);
		void						Destroy();

		void						AddToSearchPath(std::string path);

		bool						FileExists(std::string path);
		bool						FolderExists(std::string path);

		std::vector<std::string>	GetDirectoryContent(std::string path);

		long						FileSize(std::string path);

		std::string					GetFileContentString(std::string path);

		std::string					GetRealPath(std::string path);
		long						GetFileModifyTime(std::string path);
	}
}