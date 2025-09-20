#pragma once
#include <hde64.h>


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

// already tired of writing goddamn nops
template<std::size_t N>
constexpr std::array<uint8_t, N> Nop()
{
    std::array<uint8_t, N> arr{};
    arr.fill(0x90);
    return arr;
}

inline std::vector<uint8_t> Nop(std::size_t size)
{
    return std::vector<uint8_t>(size, 0x90);
}

static size_t ComputeInstrSize(uint8_t* addr, size_t minBytes = 5, size_t maxBytes = 16)
{
    size_t total = 0;

    while (total < minBytes)
    {
        hde64s hs{};
        size_t len = hde64_disasm(addr + total, &hs);

        if (len == 0)
            return minBytes;

        total += len;

        if (total > 64)
            return total;
    }

    return (total <= maxBytes) ? total : maxBytes;
}
