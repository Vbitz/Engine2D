/*
   Filename: Platform.hpp
   Purpose:  Platform specfic interfaces

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

#pragma once

#include <string>
#include <vector>

#include <stdint.h>
#include <assert.h>

#include "stdlib.hpp"

typedef void* (*ThreadMethod)(void*);

typedef void* (*ENGINE_FARPROC)(void);

// system includes
#ifdef _WIN32
#define _PLATFORM "Windows"
#define _PLATFORM_DYLINK ".dll"
#define _PLATFORM_WIN32
#else
#include <sys/time.h>
#ifdef __APPLE__
#ifdef __MACH__
#define _PLATFORM "Darwin (OSX)"
#define _PLATFORM_DYLINK ".dylib"
#define _PLATFORM_OSX
#else
#error Unknown Platform
#endif
#elif __linux__
#define _PLATFORM "Linux"
#define _PLATFORM_DYLINK ".so"
#define _PLATFORM_LINUX
#else
#error Unknown Platform
#endif
#endif

namespace Engine {
    namespace Platform {
        
        struct UUID {
            uint64_t partA, partB;
        };
        
        static_assert(sizeof(UUID) == 16, "The size of UUID must equal 16 bytes");
        
        ENGINE_CLASS(Libary);
        
        class Libary {
        public:
            virtual ~Libary() {}
            
            virtual bool IsValid() { return false; }
            
			virtual ENGINE_FARPROC GetMethod(std::string name) { return NULL; }
            virtual bool CallMethod(std::string name) { return true; }
            
            virtual std::vector<std::string> GetExportedMethods() {
                return std::vector<std::string>();
            }
        };
        
        ENGINE_CLASS(Thread);
        
        class Thread {
        public:
            virtual ~Thread() {}
            
            virtual void Terminate() = 0;
            virtual void Exit(void* ret) = 0;
            
            virtual UUID GetThreadID() = 0;
        };
        
        ENGINE_CLASS(Mutex);
        
        class Mutex {
        public:
            virtual ~Mutex() {}
            
            virtual bool SafeEnter() = 0;
            virtual void Enter() = 0;
            virtual void Exit() = 0;
        };
        
        enum class FileMode {
            Read,
            Write
        };
        
        ENGINE_CLASS(MemoryMappedFile);
        ENGINE_CLASS(MemoryMappedRegion);
        
        class MemoryMappedFile {
        public:
            virtual bool IsValid() { return false; }
            
            virtual void Close() { }
            
            virtual ~MemoryMappedFile() {
                if (this->IsValid()) this->Close();
            }
            
            virtual MemoryMappedRegionPtr MapRegion(unsigned long offset, size_t size) = 0;
            virtual void UnmapRegion(MemoryMappedRegionPtr r) = 0;
        };
        
        class MemoryMappedRegion {
        public:
            MemoryMappedRegion(MemoryMappedFilePtr parent, void* data, size_t length) : _parent(parent), _data(data), _length(length) { }
            
            template<typename T>
            inline T* Data() {
                assert(this->_data != NULL);
                return static_cast<T*>(this->_data);
            }
            
            template<typename T>
            inline T* Data(size_t offset) {
                assert(this->_data != NULL);
                assert(offset < this->_length);
                uint8_t* data = (uint8_t*) this->_data;
                data += offset;
                return static_cast<T*>((void*) data);
            }
            
            size_t Length() {
                return this->_length;
            }
            
            MemoryMappedFilePtr _parent;
            void* _data;
            size_t _length;
        };
        
        typedef struct {
            long totalVirtual, totalVirtualFree, myVirtualUsed;
            long totalPhysical, totalPhysicalFree, myPhysicalUsed;
        } engine_memory_info;
        
        void SetRawCommandLine(int argc, const char** argv);
        int GetRawCommandLineArgC();
        const char** GetRawCommandLineArgV();
        std::string GetExecutablePath();
        
        const char* GetInitalDirectory();
        
        engine_memory_info GetMemoryInfo();
        double GetMemoryUsage();
        
        int GetProcesserCount();
        
        LibaryPtr OpenLibary(std::string path);
        LibaryPtr GetThisLibary();
        
        ThreadPtr CreateThread(ThreadMethod entry, void* threadArgs);
        ThreadPtr GetCurrentThread();
        
        bool IsMainThread();
        
        MutexPtr CreateMutex();
        
        MemoryMappedFilePtr OpenMemoryMappedFile(std::string filename, FileMode mode);
        
        UUID GenerateUUID();
        std::string StringifyUUID(UUID uuidArr);
        UUID ParseUUID(std::string uuidStr);
        
        bool ShowMessageBox(std::string title, std::string text, bool modal);
        
        double GetTime();
        
        std::string GetUsername();
        
        void DumpStackTrace();
        
        int ShellExecute(std::string path);
        
        void Sleep(int timeS);
        void NanoSleep(int timeNS);
        
        long CryptBytes(unsigned char* buf, long len);
    }
}