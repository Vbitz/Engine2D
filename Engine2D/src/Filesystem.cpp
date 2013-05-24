#include "Filesystem.hpp"

namespace Engine {
	namespace Filesystem {
        bool hasSetUserDir = false;
        
		bool IsLoaded() {
			return PHYSFS_isInit();
		}

		void Init(const char* argv0) {
			PHYSFS_init(argv0);
			Logger::WriteVerbose("Filesystem Loaded");
			PHYSFS_mount("res", "/", 1);
		}

		void Destroy() {
			PHYSFS_deinit();
		}
        
        bool Mount(std::string path, std::string fsPath) {
            if (!IsLoaded()) {
                Logger::WriteError("FS not loaded");
                return false;
            }
            int result = PHYSFS_mount(path.c_str(), fsPath.c_str(), 1);
            if (result == 0) {
                std::cout << "Error Mounting " << PHYSFS_getLastError() << std::endl;
            }
            return result > 0;
        }
        
        void SetupUserDir(std::string path) {
            if (!IsLoaded()) {
                Logger::WriteError("FS not loaded");
                return;
            }
            PHYSFS_setSaneConfig("Engine2D", path.c_str(), NULL, 0, 0);
            hasSetUserDir = true;
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
        
        void Mkdir(std::string path) {
            if (!IsLoaded()) {
				Logger::WriteError("FS not loaded");
				return;
			}
            PHYSFS_mkdir(path.c_str());
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

		long FileSize(std::string path) {
			if (!IsLoaded()) {
				Logger::WriteError("FS not loaded");
				return -1;
			}
			PHYSFS_File* f = PHYSFS_openRead(path.c_str());
			long length = PHYSFS_fileLength(f);
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
			long fileSize = PHYSFS_fileLength(f);
			char* fBuffer;
			fBuffer = new char[fileSize];
			if (PHYSFS_read(f, fBuffer, sizeof(char), fileSize) != fileSize) {
				std::cout << "File Read Failed" << std::endl;
			}
			PHYSFS_close(f);
			return std::string(fBuffer);
		}
        
        void WriteFile(std::string path, std::string content) {
			if (!IsLoaded()) {
				Logger::WriteError("FS not loaded");
				return;
			}
            if (!hasSetUserDir) {
                Logger::WriteError("UserDir needs to be set to writefiles");
                return;
            }
            PHYSFS_File* f = PHYSFS_openWrite(path.c_str());
            if (!PHYSFS_write(f, content.c_str(), sizeof(char), content.length())) {
                std::cout << "File Write Failed" << std::endl;
            }
            PHYSFS_close(f);
        }

		std::string GetRealPath(std::string path) {
			const char* fontPath = PHYSFS_getRealDir(path.c_str());
			if (fontPath != NULL) {
				return std::string(fontPath) + '/' + path;
			} else {
				return "";
			} 
		}

		long GetFileModifyTime(std::string path) {
			return PHYSFS_getLastModTime(path.c_str());
		}
        
        bool HasSetUserDir() {
            return hasSetUserDir;
        }
	}
}