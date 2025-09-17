#pragma once
#include <windows.h>
#include <vector>
#include <memory>


class CavePatches
{
public:
    class Cave
    {
    public:
        Cave(uint8_t* target, std::span<const uint8_t> code);
        ~Cave();

        void Apply();
        void Restore();

        explicit operator bool() const { return m_Active; }

    private:
        uint8_t* m_Target{};
        std::unique_ptr<uint8_t[]> m_Code{};
        size_t m_CodeSize{};
        std::unique_ptr<uint8_t[]> m_Original{};
        size_t m_StolenSize{ 5 };

        void* m_CaveMemory{};
        bool m_Active{ false };
        std::mutex m_Mutex;
    };

private:
    static inline std::vector<std::shared_ptr<Cave>> m_Caves{};
    static inline std::mutex m_ListMutex;

public:
    static std::shared_ptr<Cave> Add(uint8_t* target, std::span<const uint8_t> code);
    static void Remove(std::shared_ptr<Cave>& cave);
    static void RestoreAll();
};

using CavePatch = std::shared_ptr<CavePatches::Cave>;
