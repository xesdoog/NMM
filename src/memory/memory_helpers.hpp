#pragma once

static void FlushICache(void* addr, SIZE_T len) {
    FlushInstructionCache(GetCurrentProcess(), addr, len);
}

static void* VirtualAllocNear(void* hint, SIZE_T size) {
    const uintptr_t targetAddr = reinterpret_cast<uintptr_t>(hint);
    const uintptr_t maxDelta = 0x7FFFFFFF;
    const uintptr_t start = (targetAddr > maxDelta) ? (targetAddr - maxDelta) : 0;
    const uintptr_t end = targetAddr + maxDelta;
    const SIZE_T step = 0x10000;

    for (uintptr_t addr = targetAddr; addr >= start; addr = (addr > step ? addr - step : 0)) {
        void* p = VirtualAlloc(reinterpret_cast<void*>(addr), size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        if (p)
            return p;
        if (addr == 0)
            break;
    }

    for (uintptr_t addr = targetAddr + step; addr <= end; addr += step) {
        void* p = VirtualAlloc(reinterpret_cast<void*>(addr), size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        if (p)
            return p;
    }
    return nullptr;
}
