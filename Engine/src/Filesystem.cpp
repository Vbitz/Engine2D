#include "Filesystem.hpp"

namespace Engine {
	namespace Filesystem {
		void Init(const char* argv0) {
			PHYSFS_init(argv0);
			Logger::WriteVerbose("Filesystem Loaded");
			PHYSFS_addToSearchPath("res", 1);
		}

		void Destroy() {
			PHYSFS_deinit();
		}

		void AddToSearchPath(std::string path) {
			PHYSFS_addToSearchPath(path.c_str(), 1);
		}

		bool FileExists(std::string path) {
			return PHYSFS_exists(path.c_str());
		}

		int FileSize(std::string path) {
			PHYSFS_File* f = PHYSFS_openRead(path.c_str());
			int length = PHYSFS_fileLength(f);
			PHYSFS_close(f);
			return length;
		}

		std::string GetFileContentString(std::string path) {
			if (!PHYSFS_exists(path.c_str())) {
				Logger::WriteError("File does not exist");
				return "";
			}
			PHYSFS_File* f = PHYSFS_openRead(path.c_str());
			int fileSize = PHYSFS_fileLength(f);
			char* fBuffer;
			fBuffer = new char[fileSize];
			int readData = PHYSFS_read(f, fBuffer, 1, fileSize);
			if (readData != fileSize) {
				Logger::WriteError("File read fail");
				return "";
			} else {
				return std::string(fBuffer);
			}
			PHYSFS_close(f);
		}
	}
}