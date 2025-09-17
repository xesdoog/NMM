#include "pointers.hpp"
#include "module_manager.hpp"
#include "pattern.hpp"
#include "pattern_scanner.hpp"
#include "pattern_cache.hpp"
#include "hooks/hooks.hpp"
#include "memory_helpers.hpp"
#include "game/features/self.hpp"


bool Pointers::Init() {
    PatternCache::Init();

    auto mmgr = ModuleMgr();

    if (!mmgr.LoadModules()) {
        LOG(FATAL) << "Failed to load modules from PEB.";
        return false;
	}

    const auto nms = mmgr.Get("NMS.exe");
    if (!nms) {
        LOG(FATAL) << "Could not find NMS.exe, is this No Man's Sky?";
        return false;
    }

    auto scanner = PatternScanner(nms);

    /*
        Credit to Blind Distortion on FearlessRevolution
        as well as contributors to their ct
        including our notorious friend gir489
        https://fearlessrevolution.com/viewtopic.php?t=30442
    */
    constexpr auto currency_ptrn = Pattern<"8B 88 ?? B8 00 00 89 8F 10 2F 00 00 45 33 ?? 45 0F 57 ?? 39 B7">("Currency");
    scanner.Add(currency_ptrn, [this](PointerCalculator ptr) {
        CurrencyInstruction = ptr.As<void*>();
    });

    constexpr auto life_support_ptrn = Pattern<"2B ?? 85 ?? 0F 4F ?? 8B 48 20 3B ??">("LifeSupport");
    scanner.Add(life_support_ptrn, [this](PointerCalculator ptr) {
        LifeSupportPatch = BytePatches::Add(ptr.As<std::uint8_t*>(),
            std::to_array<std::uint8_t>({ 0x90, 0x90 }));
    });

    constexpr auto jetpack_fuel_ptrn = Pattern<"?? 41 0F 59 ?? ?? 0F 59 ?? ?? 0F 5C ?? ?? 0F 11 87 54 31 00 00 ?? 41">("JetPackFuel");
    scanner.Add(jetpack_fuel_ptrn, [this](PointerCalculator ptr) {
        JetPackFuelPatch = BytePatches::Add(ptr.Add(9).As<std::uint8_t*>(),
            std::to_array<std::uint8_t>({ 0x90, 0x90, 0x90, 0x90 }));
    });

    constexpr auto jetpack_fuel_ptrn2 = Pattern<"?? 0F 5C ?? ?? 44 0F 5F ?? ?? 44 0F 11 87">("WaterJetPackFuel");
    scanner.Add(jetpack_fuel_ptrn2, [this](PointerCalculator ptr) {
        JetPackFuelPatch2 = BytePatches::Add(ptr.As<std::uint8_t*>(),
            std::to_array<std::uint8_t>({ 0x90, 0x90, 0x90, 0x90 }));
    });

    constexpr auto health_ptrn = Pattern<"41 01 8D B0 01 00 00 49 8B ?? 8B 95 10 03 00 00">("Health");
    scanner.Add(health_ptrn, [this](PointerCalculator ptr) {
        PlayerHealthPatch = BytePatches::Add(ptr.As<std::uint8_t*>(),
            std::to_array<std::uint8_t>({ 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 }));
    });

    constexpr auto stamina_ptrn = Pattern<"?? 41 0F 5E ?? ?? 0F 5C ?? ?? 0F 11 87 ?? 4E 00 00">("Stamina");
    scanner.Add(stamina_ptrn, [this](PointerCalculator ptr) {
        PlayerStaminaPatch = BytePatches::Add(ptr.Add(5).As<std::uint8_t*>(),
            std::to_array<std::uint8_t>({ 0x90, 0x90, 0x90, 0x90 }));
    });

    constexpr auto env_protection_ptrn = Pattern<"?? 0F 5E ?? 0F 28 ?? ?? 0F 58 4C BB 24">("EnvProtection");
    scanner.Add(env_protection_ptrn, [this](PointerCalculator ptr) {
        EnvProtectionPatch = BytePatches::Add(ptr.As<std::uint8_t*>(),
            std::to_array<std::uint8_t>({ 0x90, 0x90, 0x90, 0x90 }));
    });

    constexpr auto ship_shield_ptrn = Pattern<"44 0F 4D ?? 44 89 43 18 49 ?? 85 ?? ?? 00 00">("ShipShields");
    scanner.Add(ship_shield_ptrn, [this](PointerCalculator ptr) {
        StarshipShieldPatch = CavePatches::Add(ptr.As<uint8_t*>(), std::to_array<uint8_t>({
            0x44, 0x0F, 0x4D, 0xC0,
            0x41, 0x50,
            0x44, 0x8B, 0x43, 0x20,
            0x44, 0x89, 0x43, 0x18,
            0x41, 0x58 }));
    });

    constexpr auto launch_thruster_ptrn = Pattern<"2B ?? 48 8D 4C 24 78 89 47 18 48 8B">("LaunchThruster");
    scanner.Add(launch_thruster_ptrn, [this](PointerCalculator ptr) {
        LaunchThrusterPatch = BytePatches::Add(ptr.As<uint8_t*>(), std::to_array<uint8_t>({ 0x90, 0x90 }));
    });

    constexpr auto pulse_engine_ptrn = Pattern<"0F 2F ?? 76 ?? 41 FF ?? 18">("PulseEngine");
    scanner.Add(pulse_engine_ptrn, [this](PointerCalculator ptr) {
        PulseEnginePatch = BytePatches::Add(ptr.Add(5).As<uint8_t*>(), std::to_array<uint8_t>({ 0x90, 0x90, 0x90, 0x90 }));
    });

    constexpr auto hyper_drive_ptrn = Pattern<"2B ?? 85 ?? 41 0F 4E ?? 89 4B ??">("HyperDrive");
    scanner.Add(hyper_drive_ptrn, [this](PointerCalculator ptr) {
        HyperDrivePatch = BytePatches::Add(ptr.As<uint8_t*>(), std::to_array<uint8_t>({ 0x90, 0x90 }));
    });

    constexpr auto ship_laser_ptrn = Pattern<"?? 0F 5E ?? ?? 41 0F 58 8C 86 ?? ?? ?? ??">("ShipLaser");
    scanner.Add(ship_laser_ptrn, [this](PointerCalculator ptr) {
        ShipLaserPatch = BytePatches::Add(ptr.Add(4).As<uint8_t*>(),
            std::to_array<uint8_t>({ 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 }));
    });

    constexpr auto rocket_cd_ptrn = Pattern<"41 89 84 8E ?? ?? ?? ?? F3 0F">("ShipRockets");
    scanner.Add(rocket_cd_ptrn, [this](PointerCalculator ptr) {
        RocketCooldownPatch = BytePatches::Add(ptr.Sub(7).As<std::uint8_t*>(),
            std::to_array<uint8_t>({ 0xB8, 0x00, 0x00, 0x80, 0x3F, 0x90, 0x90 }));
    });

    constexpr auto inf_multitool_ptrn = Pattern<"41 29 45 ?? 01 1E B8 04 00 00 00">("Multitool");
    scanner.Add(inf_multitool_ptrn, [this](PointerCalculator ptr) {
        InfMultitoolPatch = BytePatches::Add(ptr.As<std::uint8_t*>(), std::to_array<uint8_t>({ 0x90, 0x90, 0x90, 0x90 }));
    });

    constexpr auto inf_mining_ptrn = Pattern<"41 0F 28 ?? 41 0F 28 ?? ?? 0F 58 86 ?? ?? 00 00 F3">("MiningBeam");
    scanner.Add(inf_mining_ptrn, [this](PointerCalculator ptr) {
        InfMiningBeamPatch = BytePatches::Add(ptr.Add(8).As<std::uint8_t*>(),
            std::to_array<uint8_t>({ 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 }));
    });

    constexpr auto inf_terrain_ptrn = Pattern<"?? 0F 2C ?? 2B ?? 85 ?? 0F 4E ??">("TerrainManipulator");
    scanner.Add(inf_mining_ptrn, [this](PointerCalculator ptr) {
        InfTerrainToolPatch = BytePatches::Add(ptr.Add(4).As<std::uint8_t*>(), std::to_array<uint8_t>({ 0x90, 0x90 }));
    });

    constexpr auto mtt_dmg_ptrn = Pattern<"89 7B 50 ?? 43 48 01 48 8B 4E">("MultitoolDamage");
    scanner.Add(mtt_dmg_ptrn, [this](PointerCalculator ptr) {
        auto target = ptr.As<std::uint8_t*>();
        OneHitKillsPatch = CavePatches::Add(target,
            std::to_array<uint8_t>({0xC7, 0x43, 0x50, 0xCD, 0xCC, 0xCC, 0x3D, 0xC6, 0x43, 0x48, 0x01}));
    });

    if (!scanner.Scan())
    {
        LOG(FATAL) << "Some game patterns could not be found, unloading.";
        return false;
    }

    LOG(INFO) << "Pointer scan complete.";
    PatternCache::Update();
	return true;
}

bool Pointers::LateInit() 
{
    // will be useful later
	return true;
}

void Pointers::Restore()
{
}
