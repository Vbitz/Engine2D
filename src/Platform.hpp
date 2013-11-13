#pragma once

#include <string>
#include <vector>

#ifndef FARPROC
typedef void* (*FARPROC)(void);
#endif

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
        
        typedef struct {
            long totalVirtual, totalVirtualFree, myVirtualUsed;
            long totalPhysical, totalPhysicalFree, myPhysicalUsed;
        } engine_memory_info;
        
        void SetRawCommandLine(int argc, const char** argv);
        int GetRawCommandLineArgC();
        const char** GetRawCommandLineArgV();
        
        engine_memory_info GetMemoryInfo();
        
        Libary* OpenLibary(std::string path);
        Libary* GetThisLibary();
        
        double GetTime();
    }
}