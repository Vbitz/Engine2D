#include "Filesystem.hpp"

namespace Engine {
	namespace Filesystem {
        bool hasSetUserDir = false;
        
		bool IsLoaded() {
			return PHYSFS_isInit();
		}

		void Init(const char* argv0) {
			PHYSFS_init(argv0);
			Logger::begin("Filesystem", Logger::LogLevel_Error) << "Filesystem Loaded" << std::endl;
			PHYSFS_mount("res", "/", 1);
		}

		void Destroy() {
			PHYSFS_deinit();
		}
        
        bool Mount(std::string path, std::string fsPath) {
            if (!IsLoaded()) {
                Logger::begin("Filesystem", Logger::LogLevel_Error) << "FS not loaded" << std::endl;
                return false;
            }
            int result = PHYSFS_mount(path.c_str(), fsPath.c_str(), 1);
            if (result == 0) {
                Logger::begin("Filesystem", Logger::LogLevel_Error) << "Error Mounting " << PHYSFS_getLastError() << std::endl;
            }
            return result > 0;
        }
        
        void SetupUserDir(std::string path) {
            if (!IsLoaded()) {
                Logger::begin("Filesystem", Logger::LogLevel_Error) << "FS not loaded" << std::endl;
                return;
            }
            PHYSFS_setSaneConfig("Engine2D", path.c_str(), NULL, 0, 0);
            hasSetUserDir = true;
        }

		bool FileExists(std::string path) {
            if (!IsLoaded()) {
                Logger::begin("Filesystem", Logger::LogLevel_Error) << "FS not loaded" << std::endl;
                return false;
            }
			const char* pathC = path.c_str();
			return PHYSFS_exists(pathC) && !PHYSFS_isDirectory(pathC);
		}

		bool FolderExists(std::string path) {
            if (!IsLoaded()) {
                Logger::begin("Filesystem", Logger::LogLevel_Error) << "FS not loaded" << std::endl;
                return false;
            }
			const char* pathC = path.c_str();
			return PHYSFS_exists(pathC) && PHYSFS_isDirectory(pathC);
		}
        
        void Mkdir(std::string path) {
            if (!IsLoaded()) {
                Logger::begin("Filesystem", Logger::LogLevel_Error) << "FS not loaded" << std::endl;
                return;
            }
            PHYSFS_mkdir(path.c_str());
        }

		std::vector<std::string> GetDirectoryContent(std::string path) {
			std::vector<std::string> ret;
            if (!IsLoaded()) {
                Logger::begin("Filesystem", Logger::LogLevel_Error) << "FS not loaded" << std::endl;
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
                Logger::begin("Filesystem", Logger::LogLevel_Error) << "FS not loaded" << std::endl;
                return -1;
            }
			PHYSFS_File* f = PHYSFS_openRead(path.c_str());
			long length = PHYSFS_fileLength(f);
			PHYSFS_close(f);
			return length;
		}

        char* GetFileContent(std::string path) {
            long f = 0;
            return GetFileContent(path, f);
        }
        
		char* GetFileContent(std::string path, long &fileSize) {
            if (!IsLoaded()) {
                Logger::begin("Filesystem", Logger::LogLevel_Error) << "FS not loaded" << std::endl;
                return (char*) "";
            }
			if (!PHYSFS_exists(path.c_str())) {
				Logger::begin("Filesystem", Logger::LogLevel_Error) << "File does not exist" << std::endl;
				return (char*) "";
			}
			PHYSFS_File* f = PHYSFS_openRead(path.c_str());
            long len = PHYSFS_fileLength(f);
			char* fBuffer = new char[len + 1];
            if (PHYSFS_read(f, fBuffer, sizeof(char), (PHYSFS_uint32) len) != len) {
                Logger::begin("Filesystem", Logger::LogLevel_Error) << "File Read Failed" << std::endl;
                return (char*) "";
            }
			PHYSFS_close(f);
            fBuffer[len] = 0x00;
            fileSize = len;
			return fBuffer;
		}
        
        void WriteFile(std::string path, char* content, long length) {
            if (!IsLoaded()) {
                Logger::begin("Filesystem", Logger::LogLevel_Error) << "FS not loaded" << std::endl;
                return;
            }
            if (!hasSetUserDir) {
                Logger::begin("Filesystem", Logger::LogLevel_Error) << "UserDir needs to be set to writefiles" << std::endl;
                return;
            }
            PHYSFS_File* f = PHYSFS_openWrite(path.c_str());
            if (!PHYSFS_write(f, content, sizeof(char), (unsigned int) length)) {
                Logger::begin("Filesystem", Logger::LogLevel_Error) << "File Write Failed" << std::endl;
            }
            PHYSFS_close(f);
        }
    
        void TouchFile(std::string path) {
            if (!IsLoaded()) {
                Logger::begin("Filesystem", Logger::LogLevel_Error) << "FS not loaded" << std::endl;
                return;
            }
            if (!hasSetUserDir) {
                Logger::begin("Filesystem", Logger::LogLevel_Error) << "UserDir needs to be set to touchFiles" << std::endl;
                return;
            }
            PHYSFS_File* f = PHYSFS_openWrite(path.c_str());
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