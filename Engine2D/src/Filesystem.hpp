#pragma once

#include <string>
#include <vector>
#include <iostream>

#include <physfs.h>

namespace Engine {
	namespace Filesystem {
		void Init(const char* argv0);
		void Destroy();

		void AddToSearchPath(std::string path);

		bool FileExists(std::string path);
		bool FolderExists(std::string path);

		std::vector<std::string> GetDirectoryContent(std::string path);

		int FileSize(std::string path);

		std::string GetFileContentString(std::string path);

		long GetFileModifyTime(std::string path);
	}
}