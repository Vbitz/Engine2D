#pragma once

#include "common.hpp"

#include <string>
#include <vector>

#include <physfs.h>

namespace Engine {
	namespace Filesystem {
        
		bool						IsLoaded();

		void						Init(const char* argv0);
		void						Destroy();

        bool                        Mount(std::string path, std::string fsPath);
        void                        SetupUserDir(std::string path);
        
		bool						FileExists(std::string path);
        
		bool						FolderExists(std::string path);
        
        void                        Mkdir(std::string path);
        
		std::vector<std::string>	GetDirectoryContent(std::string path);

		long						FileSize(std::string path);
        
        char*                       GetFileContent(std::string path);
        char*                       GetFileContent(std::string path, long& fileSize);
        void                        WriteFile(std::string path, std::string content);
        void                        TouchFile(std::string path);

		std::string					GetRealPath(std::string path);
		long						GetFileModifyTime(std::string path);
        
        bool                        HasSetUserDir();
	}
}