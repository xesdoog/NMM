#pragma once
#include <string_view>
#include <Windows.h>
#include <memory/memory_helpers.hpp>
#include "base_hook.hpp"


class InlineHook : public BaseHook
{
public:
    InlineHook(const std::string_view name, void* target, void* detour, size_t overwriteSize = 6);
    ~InlineHook();

    bool Enable() override;
    bool Disable() override;

private:
    void* m_Target;
    void* m_Detour;
    void* m_Stub;
    size_t m_OverwriteSize;
    BYTE m_OriginalBytes[32];
};

inline InlineHook::InlineHook(const std::string_view name, void* target, void* detour, size_t overwriteSize):
    BaseHook(name),
    m_Target(target),
    m_Detour(detour),
    m_Stub(nullptr),
    m_OverwriteSize(overwriteSize)
{
    memset(m_OriginalBytes, 0, sizeof(m_OriginalBytes));
}

inline InlineHook::~InlineHook()
{
    Disable();
}

inline bool InlineHook::Enable()
{
    if (m_Enabled)
        return true;

    if (!m_Target || !m_Detour)
    {
        LOG(FATAL) << "InlineHook: invalid target";
        return false;
    }

    BYTE* pTarget = reinterpret_cast<BYTE*>(m_Target);

    if (m_OverwriteSize == 0 || m_OverwriteSize > sizeof(m_OriginalBytes)) {
        LOG(FATAL) << "InlineHook: bad overwrite size";
        return false;
    }

    memcpy(m_OriginalBytes, pTarget, m_OverwriteSize);

    const SIZE_T stubSize = 512;
    void* stub = VirtualAllocNear(m_Target, stubSize);

    if (!stub)
        stub = VirtualAlloc(nullptr, stubSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    if (!stub)
    {
        LOG(FATAL) << "InlineHook: Failed to allocate memory for hook.";
        return false;
    }

    std::vector<uint8_t> stubBytes;
    stubBytes.reserve(256);
    uintptr_t writeAddr = reinterpret_cast<uintptr_t>(m_Detour);

    stubBytes.push_back(0x48);
    stubBytes.push_back(0x85);
    stubBytes.push_back(0xC0);
    stubBytes.push_back(0x74);
    stubBytes.push_back(0x09);
    stubBytes.push_back(0x48);
    stubBytes.push_back(0xA3);

    for (int i = 0; i < 8; ++i)
        stubBytes.push_back((writeAddr >> (i * 8)) & 0xFF);

    for (SIZE_T i = 0; i < m_OverwriteSize; ++i)
        stubBytes.push_back(m_OriginalBytes[i]);

    stubBytes.push_back(0xFF);
    stubBytes.push_back(0x25);
    stubBytes.push_back(0x00);
    stubBytes.push_back(0x00);
    stubBytes.push_back(0x00);
    stubBytes.push_back(0x00);

    uintptr_t retAddr = reinterpret_cast<uintptr_t>(pTarget + m_OverwriteSize);
    for (int i = 0; i < 8; ++i)
        stubBytes.push_back((retAddr >> (i * 8)) & 0xFF);

    DWORD oldProt;
    if (!VirtualProtect(stub, stubSize, PAGE_EXECUTE_READWRITE, &oldProt))
    {
        VirtualFree(stub, 0, MEM_RELEASE);
        LOG(FATAL) << "InlineHook: VirtualProtect(stub) failed";
        return false;
    }

    memcpy(stub, stubBytes.data(), stubBytes.size());
    FlushInstructionCache(GetCurrentProcess(), stub, stubBytes.size());
    intptr_t rel = reinterpret_cast<intptr_t>(stub) - reinterpret_cast<intptr_t>(pTarget) - 5;

    if (rel < INT32_MIN || rel > INT32_MAX) {
        VirtualFree(stub, 0, MEM_RELEASE);
        LOG(FATAL) << "InlineHook: stub out of relative jmp range";
        return false;
    }

    if (!VirtualProtect(pTarget, m_OverwriteSize, PAGE_EXECUTE_READWRITE, &oldProt))
    {
        VirtualFree(stub, 0, MEM_RELEASE);
        LOG(FATAL) << "InlineHook: VirtualProtect(target) failed";
        return false;
    }

    pTarget[0] = 0xE9;
    *reinterpret_cast<int32_t*>(pTarget + 1) = static_cast<int32_t>(rel);
    for (SIZE_T i = 5; i < m_OverwriteSize; ++i)
        pTarget[i] = 0x90;

    FlushInstructionCache(GetCurrentProcess(), pTarget, m_OverwriteSize);
    VirtualProtect(pTarget, m_OverwriteSize, oldProt, &oldProt);
    m_Stub = stub;
    m_Enabled = true;

    return true;
}

inline bool InlineHook::Disable()
{
    if (!m_Enabled)
        return true;

    if (!m_Target || !m_Stub)
    {
        m_Enabled = false;
        return false;
    }

    BYTE* pTarget = reinterpret_cast<BYTE*>(m_Target);
    DWORD oldProt;
    if (!VirtualProtect(pTarget, m_OverwriteSize, PAGE_EXECUTE_READWRITE, &oldProt))
    {
        LOG(FATAL) << "InlineHook: VirtualProtect(target) failed during Disable";
        return false;
    }

    memcpy(pTarget, m_OriginalBytes, m_OverwriteSize);
    FlushInstructionCache(GetCurrentProcess(), pTarget, m_OverwriteSize);
    VirtualProtect(pTarget, m_OverwriteSize, oldProt, &oldProt);

    VirtualFree(m_Stub, 0, MEM_RELEASE);
    m_Stub = nullptr;
    m_Enabled = false;
    return true;
}

