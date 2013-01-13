#include "Filesystem.hpp"

namespace Engine {
	namespace Filesystem {
		void Init(const char* argv0) {
			PHYSFS_init(argv0);
			PHYSFS_addToSearchPath("res", 1);
		}

		void Destroy() {
			PHYSFS_deinit();
		}

		void AddToSearchPath(std::string path) {
			PHYSFS_addToSearchPath(path.c_str(), 1);
		}

		bool FileExists(std::string path) {
			const char* pathC = path.c_str();
			return PHYSFS_exists(pathC) && !PHYSFS_isDirectory(pathC);
		}

		bool FolderExists(std::string path) {
			const char* pathC = path.c_str();
			return PHYSFS_exists(pathC) && PHYSFS_isDirectory(pathC);
		}

		std::vector<std::string> GetDirectoryContent(std::string path) {
			std::vector<std::string> ret;
			char** list = PHYSFS_enumerateFiles(path.c_str());
			char** i;
			for (i = list; *i != NULL; i++) {
				ret.push_back(*i);
			}
			PHYSFS_freeList(list);
			return ret;
		}

		int FileSize(std::string path) {
			PHYSFS_File* f = PHYSFS_openRead(path.c_str());
			int length = PHYSFS_fileLength(f);
			PHYSFS_close(f);
			return length;
		}

		std::string GetFileContentString(std::string path) {
			if (!FileExists(path)) {
				std::cout << "File does not exist" << std::endl;
				return "";
			}
			PHYSFS_File* f = PHYSFS_openRead(path.c_str());
			int fileSize = PHYSFS_fileLength(f);
			char* fBuffer;
			fBuffer = new char[fileSize];
			int readData = PHYSFS_read(f, fBuffer, 1, fileSize);
			PHYSFS_close(f);
			if (readData != fileSize) {
				return "";
			} else {
				return std::string(fBuffer);
			}
			
		}

		long GetFileModifyTime(std::string path) {
			if (!FileExists(path)) {
				std::cout << "File does not exist" << std::endl;
				return -1;
			}
			return PHYSFS_getLastModTime(path.c_str());
		}
	}
}