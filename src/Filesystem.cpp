/*
   Filename: Filesystem.cpp
   Purpose:  C++ abstraction over PhysFS

   Part of Engine2D

   Copyright (C) 2014 Vbitz

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "Filesystem.hpp"

#include "vendor/physfs/physfs.h"

namespace Engine {
	namespace Filesystem {
        bool hasSetUserDir = false;
        
        class PhysFSFile : public File {
        public:
            PhysFSFile(PHYSFS_File* file, FileMode mode) : _file(file), _mode(mode) {}
            
            ~PhysFSFile() override {
                if (!this->_closed) {
                    this->Close();
                }
            }
            
            void Close() override {
                if (this->_closed) return;
                PHYSFS_close(this->_file);
            }
            
            FileMode GetMode() override {
                
            }
            
        private:
            bool _closed = false;
            PHYSFS_File* _file = NULL;
            FileMode _mode;
        };
        
        FilePtr File::Open(std::string path, FileMode mode) {
            PHYSFS_File* file = NULL;
            switch (mode) {
                case FileMode::Read:
                    file = PHYSFS_openRead(path.c_str());
                    break;
                case FileMode::Write:
                    file = PHYSFS_openWrite(path.c_str());
                    break;
                case FileMode::Append:
                    file = PHYSFS_openAppend(path.c_str());
                    break;
            }
            return new PhysFSFile(file, mode);
        }
        
		bool IsLoaded() {
			return PHYSFS_isInit();
		}

		void Init(const char* argv0) {
			PHYSFS_init(argv0);
			Logger::begin("Filesystem", Logger::LogLevel_Log) << "Filesystem Loaded" << Logger::end();
			PHYSFS_mount("res", "/", 1);
		}

		void Destroy() {
			PHYSFS_deinit();
		}
        
        bool Mount(std::string path, std::string fsPath) {
            if (!IsLoaded()) {
                Logger::begin("Filesystem", Logger::LogLevel_Error) << "FS not loaded" << Logger::end();
                return false;
            }
            int result = PHYSFS_mount(path.c_str(), fsPath.c_str(), 1);
            if (result == 0) {
                Logger::begin("Filesystem", Logger::LogLevel_Error) << "Error Mounting " << PHYSFS_getLastError() << Logger::end();
            }
            return result > 0;
        }
        
        bool Mount(std::string path) {
            return Mount(path, "");
        }
        
        void SetupUserDir(std::string path) {
            if (HasSetUserDir()) return; // Don't call PHYSFS_setSaneConfig twice
            if (!IsLoaded()) {
                Logger::begin("Filesystem", Logger::LogLevel_Error) << "FS not loaded" << Logger::end();
                return;
            }
            PHYSFS_setSaneConfig("Engine2D", path.c_str(), NULL, 0, 0);
            hasSetUserDir = true;
        }

		bool FileExists(std::string path) {
            if (!IsLoaded()) {
                Logger::begin("Filesystem", Logger::LogLevel_Error) << "FS not loaded" << Logger::end();
                return false;
            }
			const char* pathC = path.c_str();
			return PHYSFS_exists(pathC) && !PHYSFS_isDirectory(pathC);
		}

		bool FolderExists(std::string path) {
            if (!IsLoaded()) {
                Logger::begin("Filesystem", Logger::LogLevel_Error) << "FS not loaded" << Logger::end();
                return false;
            }
			const char* pathC = path.c_str();
			return PHYSFS_exists(pathC) && PHYSFS_isDirectory(pathC);
		}
        
        void Mkdir(std::string path) {
            if (!IsLoaded()) {
                Logger::begin("Filesystem", Logger::LogLevel_Error) << "FS not loaded" << Logger::end();
                return;
            }
            PHYSFS_mkdir(path.c_str());
        }

		std::vector<std::string> GetDirectoryContent(std::string path) {
			std::vector<std::string> ret;
            if (!IsLoaded()) {
                Logger::begin("Filesystem", Logger::LogLevel_Error) << "FS not loaded" << Logger::end();
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
                Logger::begin("Filesystem", Logger::LogLevel_Error) << "FS not loaded" << Logger::end();
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
                Logger::begin("Filesystem", Logger::LogLevel_Error) << "FS not loaded" << Logger::end();
                return (char*) "";
            }
			if (!PHYSFS_exists(path.c_str())) {
				Logger::begin("Filesystem", Logger::LogLevel_Error) << "File does not exist : " << path << Logger::end();
				return (char*) "";
			}
			PHYSFS_File* f = PHYSFS_openRead(path.c_str());
            long len = PHYSFS_fileLength(f);
			char* fBuffer = new char[len + 1];
            if (PHYSFS_read(f, fBuffer, sizeof(char), (PHYSFS_uint32) len) != len) {
                Logger::begin("Filesystem", Logger::LogLevel_Error) << "File Read Failed" << Logger::end();
                return (char*) "";
            }
			PHYSFS_close(f);
            fBuffer[len] = 0x00;
            fileSize = len;
			return fBuffer;
		}
        
        std::string GetFileHexDigest(Hash::DigestType type, std::string path) {
            long fileSize = 0;
            char* fileContent = GetFileContent(path);
            std::string hexDigest = Hash::HexDigest(type, (uint8_t*) fileContent, (size_t) fileSize);
            delete [] fileContent;
            return hexDigest;
        }
        
        void WriteFile(std::string path, const char* content, long length) {
            if (!IsLoaded()) {
                Logger::begin("Filesystem", Logger::LogLevel_Error) << "FS not loaded" << Logger::end();
                return;
            }
            if (!hasSetUserDir) {
                Logger::begin("Filesystem", Logger::LogLevel_Error) << "UserDir needs to be set to writefiles" << Logger::end();
                return;
            }
            PHYSFS_File* f = PHYSFS_openWrite(path.c_str());
            if (!PHYSFS_write(f, content, sizeof(char), (unsigned int) length)) {
                Logger::begin("Filesystem", Logger::LogLevel_Error) << "File Write Failed" << Logger::end();
            }
            PHYSFS_close(f);
        }
    
        void TouchFile(std::string path) {
            if (!IsLoaded()) {
                Logger::begin("Filesystem", Logger::LogLevel_Error) << "FS not loaded" << Logger::end();
                return;
            }
            if (!hasSetUserDir) {
                Logger::begin("Filesystem", Logger::LogLevel_Error) << "UserDir needs to be set to touchFiles" << Logger::end();
                return;
            }
            PHYSFS_File* f = PHYSFS_openWrite(path.c_str());
            PHYSFS_close(f);
        }
        
        void DeleteFile(std::string path) {
            if (!IsLoaded()) {
                Logger::begin("Filesystem", Logger::LogLevel_Error) << "FS not loaded" << Logger::end();
                return;
            }
            if (!hasSetUserDir) {
                Logger::begin("Filesystem", Logger::LogLevel_Error) << "UserDir needs to be set to touchFiles" << Logger::end();
                return;
            }
            PHYSFS_delete(path.c_str());
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