#pragma once

namespace Engine {
    namespace Platform {
        
        typedef struct {
            long totalVirtual, totalVirtualFree, myVirtualUsed;
            long totalPhysical, totalPhysicalFree, myPhysicalUsed;
        } engine_memory_info;
        
        engine_memory_info GetMemoryInfo();
        
    }
}