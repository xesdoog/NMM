#include "pointers.hpp"
#include "module_manager.hpp"
#include "pattern.hpp"
#include "pattern_scanner.hpp"
#include "hooks/hooks.hpp"
#include "logging/logger.hpp"


bool Pointers::Init() {
    auto mmgr = ModuleMgr();

    if (!mmgr.LoadModules()) {
        Logger::Log(ERR, "Failed to load modules from PEB.");
        return false;
	}

    const auto nms = mmgr.Get("NMS.exe");
    if (!nms) {
        Logger::Log(ERR, "Could not find NMS.exe, is this No Man's Sky?");
        return false;
    }

    auto scanner = PatternScanner(nms);

    constexpr auto currency_ptrn = Pattern<"8B 88 C0 B8 00 00 89 8F">("CurrencyInstruction");
    scanner.Add(currency_ptrn, [this](PointerCalculator ptr) {
        CurrencyInstruction = ptr.As<void*>();
        Logger::Log(INFO, std::format("Currency instruction found at 0x{:X}", (uintptr_t)CurrencyInstruction));
    });

    if (!scanner.Scan())
    {
        Logger::Log(ERR, "Some game patterns could not be found, unloading.");
        return false;
    }

    Logger::Log(INFO, "Initial pointer scan complete");
	return true;
}

bool Pointers::LateInit() 
{
	return true;
}

void Pointers::Restore()
{
}
