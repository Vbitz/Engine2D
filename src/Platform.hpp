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

#ifndef FARPROC
typedef void* (*FARPROC)(void);
#endif

typedef void* (*ThreadMethod)(void*);

namespace Engine {
    namespace Platform {
        
        class Libary {
        public:
            virtual ~Libary() {}
            
            virtual bool IsValid() { return false; }
            
            virtual FARPROC GetMethod(std::string name) { return NULL; }
            virtual bool CallMethod(std::string name) { return 2; }
            
            virtual std::vector<std::string> GetExportedMethods() {
                return std::vector<std::string>();
            }
        };
        
        class Thread {
        public:
            virtual ~Thread() {}
            
            virtual void Terminate() = 0;
            virtual void Exit(void* ret) = 0;
            
            virtual unsigned char* GetThreadID() = 0;
        };
        
        class Mutex {
        public:
            virtual ~Mutex() {}
            
            virtual bool SafeEnter() = 0;
            virtual void Enter() = 0;
            virtual void Exit() = 0;
        };
        
        typedef struct {
            long totalVirtual, totalVirtualFree, myVirtualUsed;
            long totalPhysical, totalPhysicalFree, myPhysicalUsed;
        } engine_memory_info;
        
        void SetRawCommandLine(int argc, const char** argv);
        int GetRawCommandLineArgC();
        const char** GetRawCommandLineArgV();
        
        engine_memory_info GetMemoryInfo();
        
        int GetProcesserCount();
        
        Libary* OpenLibary(std::string path);
        Libary* GetThisLibary();
        
        Thread* CreateThread(ThreadMethod entry, void* threadArgs);
        Thread* GetCurrentThread();
        
        Mutex* CreateMutex();
        
        unsigned char* GenerateUUID();
        std::string StringifyUUID(unsigned char* uuidArr);
        unsigned char* ParseUUID(std::string uuidStr);
        
        bool ShowMessageBox(std::string title, std::string text, bool modal);
        
        double GetTime();
        
        std::string GetUsername();
        
        void DumpStackTrace();
        
        int ShellExecute(std::string path);
        
        void Sleep(int timeS);
        void NanoSleep(int timeNS);
    }
}