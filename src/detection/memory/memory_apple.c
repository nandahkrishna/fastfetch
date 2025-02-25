#include "memory.h"

#include <string.h>
#include <mach/mach.h>
#include <sys/sysctl.h>

void ffDetectMemory(FFMemoryStorage* ram)
{
    size_t length = sizeof(ram->bytesTotal);
    if (sysctl((int[]){ CTL_HW, HW_MEMSIZE }, 2, &ram->bytesTotal, &length, NULL, 0))
    {
        ffStrbufAppendS(&ram->error, "Failed to read hw.memsize");
        return;
    }

    uint32_t pagesize;
    length = sizeof(pagesize);
    if (sysctl((int[]){ CTL_HW, HW_PAGESIZE }, 2, &pagesize, &length, NULL, 0))
    {
        ffStrbufAppendS(&ram->error, "Failed to read hw.pagesize");
        return;
    }

    mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
    vm_statistics64_data_t vmstat;
    if(host_statistics64(mach_host_self(), HOST_VM_INFO64, (host_info64_t) (&vmstat), &count) != KERN_SUCCESS)
    {
        ffStrbufAppendS(&ram->error, "Failed to read host_statistics64");
        return;
    }

    ram->bytesUsed = ((uint64_t) vmstat.active_count + vmstat.wire_count) * pagesize;
}
