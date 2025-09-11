#include "currency_hook.hpp"
#include "memory/pointers.hpp"


static constexpr SIZE_T kOverwriteSize = 6;
static BYTE g_originalBytes[kOverwriteSize] = { 0 };
static void* g_stub = nullptr;
static void* g_target = nullptr;

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
        if (p) return p;
        if (addr == 0) break;
    }
    for (uintptr_t addr = targetAddr + step; addr <= end; addr += step) {
        void* p = VirtualAlloc(reinterpret_cast<void*>(addr), size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        if (p) return p;
    }
    return nullptr;
}

// TODO: make this part of Hooks as InlineHook
bool InstallCurrencyInlineHook()
{
    // Credit to Blind Distorian on FearlessRevolution as well as contributors to their ct
    // including our notorious friend gir489.
    // I have no idea what the fuck I'm doing.
    // https://fearlessrevolution.com/viewtopic.php?t=30442

    if (!g_pointers.CurrencyInstruction)
        return false;

    g_target = g_pointers.CurrencyInstruction;
    BYTE* pTarget = reinterpret_cast<BYTE*>(g_pointers.CurrencyInstruction);
    memcpy(g_originalBytes, pTarget, kOverwriteSize);
    const SIZE_T stubSize = 256; // Is this enough? I guess we'll find out
    void* stub = VirtualAllocNear(g_pointers.CurrencyInstruction, stubSize);

    if (!stub) {
        // then just VirtualAlloc like a sane person
        stub = VirtualAlloc(nullptr, stubSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        if (!stub) {
            return false;
        }
    }
    g_stub = stub;
    std::vector<BYTE> stubBytes;
    stubBytes.reserve(256);
    uintptr_t gPlayerPtrAddr = reinterpret_cast<uintptr_t>(&g_pointers.PlayerCurrency);
    stubBytes.push_back(0x48);
    stubBytes.push_back(0xA3);
    for (int i = 0; i < 8; ++i)
        stubBytes.push_back((BYTE)((gPlayerPtrAddr >> (i * 8)) & 0xFF));

    for (SIZE_T i = 0; i < kOverwriteSize; ++i)
        stubBytes.push_back(g_originalBytes[i]);

    stubBytes.push_back(0xFF);
    stubBytes.push_back(0x25);
    stubBytes.push_back(0x00);
    stubBytes.push_back(0x00);
    stubBytes.push_back(0x00);
    stubBytes.push_back(0x00);
    uintptr_t retAddr = reinterpret_cast<uintptr_t>(pTarget + kOverwriteSize);

    for (int i = 0; i < 8; ++i)
        stubBytes.push_back((BYTE)((retAddr >> (i * 8)) & 0xFF));

    DWORD old;
    if (!VirtualProtect(stub, stubSize, PAGE_EXECUTE_READWRITE, &old)) {
        VirtualFree(stub, 0, MEM_RELEASE);
        g_stub = nullptr;
        return false;
    }

    memcpy(stub, stubBytes.data(), stubBytes.size());
    FlushICache(stub, stubBytes.size());
    intptr_t rel = reinterpret_cast<intptr_t>(stub) - reinterpret_cast<intptr_t>(pTarget) - 5;

    if (rel < INT32_MIN || rel > INT32_MAX) {
        VirtualFree(stub, 0, MEM_RELEASE);
        g_stub = nullptr;
        return false;
    }

    if (!VirtualProtect(pTarget, kOverwriteSize, PAGE_EXECUTE_READWRITE, &old)) {
        VirtualFree(stub, 0, MEM_RELEASE);
        g_stub = nullptr;
        return false;
    }

    pTarget[0] = 0xE9;
    *reinterpret_cast<int32_t*>(pTarget+1) = static_cast<int32_t>(rel);

    for (SIZE_T i = 5; i < kOverwriteSize; ++i)
        pTarget[i] = 0x90;

    FlushICache(pTarget, kOverwriteSize);
    VirtualProtect(pTarget, kOverwriteSize, old, &old);

    return true;
}

bool UninstallCurrencyInlineHook()
{
    if (!g_target || !g_stub)
        return false;

    BYTE* pTarget = reinterpret_cast<BYTE*>(g_target);
    DWORD old;

    if (!VirtualProtect(pTarget, kOverwriteSize, PAGE_EXECUTE_READWRITE, &old))
        return false;

    memcpy(pTarget, g_originalBytes, kOverwriteSize);
    FlushICache(pTarget, kOverwriteSize);
    VirtualProtect(pTarget, kOverwriteSize, old, &old);
    VirtualFree(g_stub, 0, MEM_RELEASE);
    g_stub = nullptr;
    g_target = nullptr;
    g_pointers.PlayerCurrency = nullptr;

    return true;
}
