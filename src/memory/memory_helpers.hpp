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

static uintptr_t AlignDown(uintptr_t addr, size_t pageSize) {
    return addr & ~(pageSize - 1);
}

static size_t AlignUpSize(uintptr_t addr, size_t len, size_t pageSize) {
    uintptr_t end = addr + len;
    uintptr_t pageStart = AlignDown(addr, pageSize);
    uintptr_t pageEnd = (end + pageSize - 1) & ~(pageSize - 1);
    return pageEnd - pageStart;
}
