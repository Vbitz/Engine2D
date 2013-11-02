#include "Platform.hpp"

#include <mach/mach.h>

#include <sys/types.h>
#include <sys/sysctl.h>

#include <mach/vm_statistics.h>

namespace Engine {
    namespace Platform {
        
        engine_memory_info GetMemoryInfo() {
            engine_memory_info ret;
            
            xsw_usage vmusage = {0};
            size_t size = sizeof(vmusage);
            if (sysctlbyname("vm.swapusage", &vmusage, &size, NULL, 0) != 0) {
                ret.totalVirtual = -1;
                ret.totalVirtualFree = -1;
            } else {
                ret.totalVirtual = vmusage.xsu_total;
                ret.totalVirtualFree = vmusage.xsu_avail;
            }
            
            int64_t totalMemory = 0;
            size_t totalMemoryLength = sizeof(totalMemory);
            if (sysctlbyname("hw.memsize", &totalMemory, &totalMemoryLength, NULL, 0) != 0) {
                ret.totalPhysical = -1;
            } else {
                ret.totalPhysical = totalMemory;
            }
            
            vm_size_t page_size;
            mach_port_t mach_port = mach_host_self();
            vm_statistics_data_t vm_stats;
            mach_msg_type_number_t count = sizeof(vm_stats) / sizeof(natural_t);
            
            if (KERN_SUCCESS == host_page_size(mach_port, &page_size) &&
                KERN_SUCCESS == host_statistics(mach_port, HOST_VM_INFO,
                                                (host_info_t)&vm_stats, &count)) {
                ret.totalPhysicalFree = (int64_t) vm_stats.free_count * (int64_t) page_size;
            } else {
                ret.totalPhysicalFree = -1;
            }
            
            struct task_basic_info t_info;
            mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;
            if (KERN_SUCCESS != task_info(mach_task_self(), TASK_BASIC_INFO,
                                          (task_info_t)&t_info, &t_info_count)) {
                ret.myVirtualUsed = -1;
                ret.myPhysicalUsed = -1;
            } else {
                ret.myVirtualUsed = t_info.virtual_size;
                ret.myPhysicalUsed = t_info.resident_size;
            }
            
            return ret;
        }
        
    }
}