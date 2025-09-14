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

    /*
        Credit to Blind Distortion on FearlessRevolution
        as well as contributors to their ct
        including our notorious friend gir489
        https://fearlessrevolution.com/viewtopic.php?t=30442
    */
    constexpr auto currency_ptrn = Pattern<"8B 88 C0 B8 00 00 89 8F">("CurrencyInstruction");
    scanner.Add(currency_ptrn, [this](PointerCalculator ptr) {
        CurrencyInstruction = ptr.As<void*>();
    });

    constexpr auto life_support_ptrn = Pattern<"3B D1 0F 4C CA 89 48 ?? 48">("LifeSupportInstruction");
    scanner.Add(life_support_ptrn, [this](PointerCalculator ptr) {
        LifeSupportInstruction = ptr.Add(5).As<void*>();
    });

    constexpr auto jetpack_fuel_ptrn = Pattern<"F3 41 0F 59 CB F3 0F 59 ?? F3 0F 5C ??">("JetPackFuel");
    scanner.Add(jetpack_fuel_ptrn, [this](PointerCalculator ptr) {
        JetPackFuelPatch = BytePatches::Add(ptr.Add(9).As<std::uint8_t*>(), std::to_array<std::uint8_t>({ 0x90, 0x90, 0x90, 0x90 }));
    });

    constexpr auto jetpack_fuel_ptrn2 = Pattern<"F3 0F 5C CA F3 44 0F 5F ?? F3 44 0F 11">("WaterJetPackFuel");
    scanner.Add(jetpack_fuel_ptrn2, [this](PointerCalculator ptr) {
        JetPackFuelPatch2 = BytePatches::Add(ptr.As<std::uint8_t*>(), std::to_array<std::uint8_t>({ 0x90, 0x90, 0x90, 0x90 }));
    });

    constexpr auto stamina_ptrn = Pattern<"F3 41 0F 5E F0 F3 0F 5C C6">("Stamina");
    scanner.Add(stamina_ptrn, [this](PointerCalculator ptr) {
        PlayerStaminaPatch = BytePatches::Add(ptr.Add(5).As<std::uint8_t*>(), std::to_array<std::uint8_t>({ 0x90, 0x90, 0x90, 0x90 }));
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
