#include "Filesystem.hpp"

namespace Engine {
	namespace Filesystem {
		bool IsLoaded() {
			return PHYSFS_isInit();
		}

		void Init(const char* argv0) {
			PHYSFS_init(argv0);
			Logger::WriteVerbose("Filesystem Loaded");
			PHYSFS_addToSearchPath("res", 1);
		}

		void Destroy() {
			PHYSFS_deinit();
		}

		void AddToSearchPath(std::string path) {
			if (!IsLoaded()) {
				Logger::WriteError("FS not loaded");
				return;
			}
			PHYSFS_addToSearchPath(path.c_str(), 1);
		}

		bool FileExists(std::string path) {
			if (!IsLoaded()) {
				Logger::WriteError("FS not loaded");
				return false;
			}
			const char* pathC = path.c_str();
			return PHYSFS_exists(pathC) && !PHYSFS_isDirectory(pathC);
		}

		bool FolderExists(std::string path) {
			if (!IsLoaded()) {
				Logger::WriteError("FS not loaded");
				return false;
			}
			const char* pathC = path.c_str();
			return PHYSFS_exists(pathC) && PHYSFS_isDirectory(pathC);
		}

		std::vector<std::string> GetDirectoryContent(std::string path) {
			std::vector<std::string> ret;
			if (!IsLoaded()) {
				Logger::WriteError("FS not loaded");
				return ret;
			}
			char** list = PHYSFS_enumerateFiles(path.c_str());
			char** i;
			for (i = list; *i != NULL; i++) {
				ret.push_back(*i);
			}
			PHYSFS_freeList(list);
			return ret;
		}

		int FileSize(std::string path) {
			if (!IsLoaded()) {
				Logger::WriteError("FS not loaded");
				return -1;
			}
			PHYSFS_File* f = PHYSFS_openRead(path.c_str());
			int length = PHYSFS_fileLength(f);
			PHYSFS_close(f);
			return length;
		}

		std::string GetFileContentString(std::string path) {
			if (!IsLoaded()) {
				Logger::WriteError("FS not loaded");
				return "";
			}
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