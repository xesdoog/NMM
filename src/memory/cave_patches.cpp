#include "cave_patches.hpp"
#include "memory_helpers.hpp"


CavePatches::Cave::Cave(uint8_t* target, std::span<const uint8_t> code)
    : m_Target(target)
{
    m_CodeSize = code.size();
    m_Code = std::make_unique<uint8_t[]>(m_CodeSize);
    memcpy(m_Code.get(), code.data(), m_CodeSize);

    m_Original = std::make_unique<uint8_t[]>(m_StolenSize);
    memcpy(m_Original.get(), m_Target, m_StolenSize);
}

CavePatches::Cave::~Cave()
{
    Restore();
}

void CavePatches::Cave::Apply()
{
    if (m_Active || !g_Running)
        return;

    std::lock_guard lock(m_Mutex);

    if (m_StolenSize < 5)
    {
        LOG(FATAL) << "Failed to get enough bytes for code cave!";
        return;
    }

    size_t allocSize = m_CodeSize + 16;
    m_CaveMemory = VirtualAlloc(nullptr, allocSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!m_CaveMemory) {
        LOG(FATAL) << "Failed to allocate memory for code cave patch!";
        return;
    }

    std::memcpy(m_CaveMemory, m_Code.get(), m_CodeSize);
    uint8_t* caveEnd = reinterpret_cast<uint8_t*>(m_CaveMemory) + m_CodeSize;
    intptr_t relBack = (reinterpret_cast<intptr_t>(m_Target) + m_StolenSize) - (reinterpret_cast<intptr_t>(caveEnd) + 5);
    caveEnd[0] = 0xE9;
    *reinterpret_cast<int32_t*>(caveEnd + 1) = static_cast<int32_t>(relBack);
    std::memcpy(m_Original.get(), m_Target, m_StolenSize);
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    const size_t pageSize = si.dwPageSize;
    uintptr_t targetPage = AlignDown(reinterpret_cast<uintptr_t>(m_Target), pageSize);
    size_t protectSize = AlignUpSize(reinterpret_cast<uintptr_t>(m_Target), m_StolenSize, pageSize);
    DWORD oldProt;

    if (!VirtualProtect(reinterpret_cast<LPVOID>(targetPage), protectSize, PAGE_EXECUTE_READWRITE, &oldProt))
    {
        VirtualFree(m_CaveMemory, 0, MEM_RELEASE);
        m_CaveMemory = nullptr;
        return;
    }

    uint8_t* t = m_Target;
    intptr_t relToCave = reinterpret_cast<intptr_t>(m_CaveMemory) - (reinterpret_cast<intptr_t>(m_Target) + 5);
    t[0] = 0xE9;
    *reinterpret_cast<int32_t*>(t + 1) = static_cast<int32_t>(relToCave);

    for (size_t i = 5; i < m_StolenSize; ++i)
        t[i] = 0x90;

    DWORD tmp;
    VirtualProtect(reinterpret_cast<LPVOID>(targetPage), protectSize, oldProt, &tmp);
    FlushInstructionCache(GetCurrentProcess(), m_CaveMemory, allocSize);
    FlushInstructionCache(GetCurrentProcess(), m_Target, m_StolenSize);

    m_Active = true;
}

void CavePatches::Cave::Restore()
{
    if (!m_Active)
        return;

    std::lock_guard lock(m_Mutex);
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    const size_t pageSize = si.dwPageSize;
    uintptr_t targetPage = AlignDown(reinterpret_cast<uintptr_t>(m_Target), pageSize);
    size_t protectSize = AlignUpSize(reinterpret_cast<uintptr_t>(m_Target), m_StolenSize, pageSize);
    DWORD oldProt;
    VirtualProtect(reinterpret_cast<LPVOID>(targetPage), protectSize, PAGE_EXECUTE_READWRITE, &oldProt);
    std::memcpy(m_Target, m_Original.get(), m_StolenSize);
    DWORD tmp;
    VirtualProtect(reinterpret_cast<LPVOID>(targetPage), protectSize, oldProt, &tmp);
    FlushInstructionCache(GetCurrentProcess(), m_Target, m_StolenSize);

    if (m_CaveMemory) {
        VirtualFree(m_CaveMemory, 0, MEM_RELEASE);
        m_CaveMemory = nullptr;
    }

    m_Active = false;
}

std::shared_ptr<CavePatches::Cave> CavePatches::Add(uint8_t* target, std::span<const uint8_t> code)
{
    std::lock_guard lock(m_ListMutex);
    auto cave = std::make_shared<Cave>(target, code);
    m_Caves.push_back(cave);
    return cave;
}

void CavePatches::Remove(std::shared_ptr<Cave>& cave)
{
    if (!cave)
        return;

    cave->Restore();

    std::lock_guard lock(m_ListMutex);
    if (auto it = std::find(m_Caves.begin(), m_Caves.end(), cave); it != m_Caves.end())
        m_Caves.erase(it);

    cave.reset();
}

void CavePatches::RestoreAll()
{
    std::lock_guard lock(m_ListMutex);
    for (auto& cave : m_Caves)
        cave->Restore();
    m_Caves.clear();
}
