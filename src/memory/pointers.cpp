#include "pointers.hpp"
#include "module_manager.hpp"
#include "pattern.hpp"
#include "pattern_scanner.hpp"
#include "pattern_cache.hpp"
#include "hooks/hooks.hpp"
#include "memory_helpers.hpp"
#include "game/features/self.hpp"


bool Pointers::Init()
{
    PatternCache::Init();

    auto mmgr = ModuleMgr();
    if (!mmgr.LoadModules())
    {
        LOG(FATAL) << "Failed to load modules from PEB.";
        return false;
	}

    const auto nms = mmgr.Get("NMS.exe");
    if (!nms)
    {
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

    // should probably also compute instruction size if we're nopping the whole thing.
    // at least one intruction size decreased in the voyagers update which leaked the last nop into the next instruction.
    constexpr auto life_support_ptrn = Pattern<"2B ?? 85 ?? 0F 4F ?? 8B 48 20 3B ??">("LifeSupport");
    scanner.Add(life_support_ptrn, [this](PointerCalculator ptr) {
        LifeSupportPatch = BytePatches::Add(ptr.As<std::uint8_t*>(), Nop<2>());
    });

    constexpr auto jetpack_fuel_ptrn = Pattern<"?? 41 0F 59 ?? ?? 0F 59 ?? ?? 0F 5C ?? ?? 0F 11 87 54 31 00 00 ?? 41">("JetPackFuel");
    scanner.Add(jetpack_fuel_ptrn, [this](PointerCalculator ptr) {
        JetPackFuelPatch = BytePatches::Add(ptr.Add(9).As<std::uint8_t*>(), Nop<4>());
    });

    constexpr auto jetpack_fuel_ptrn2 = Pattern<"?? 0F 5C ?? ?? 44 0F 5F ?? ?? 44 0F 11 87">("WaterJetPackFuel");
    scanner.Add(jetpack_fuel_ptrn2, [this](PointerCalculator ptr) {
        JetPackFuelPatch2 = BytePatches::Add(ptr.As<std::uint8_t*>(), Nop<4>());
    });

    constexpr auto health_ptrn = Pattern<"41 01 8D B0 01 00 00 49 8B ?? 8B 95 10 03 00 00">("Health");
    scanner.Add(health_ptrn, [this](PointerCalculator ptr) {
        PlayerHealthPatch = BytePatches::Add(ptr.As<std::uint8_t*>(), Nop<7>());
    });

    constexpr auto stamina_ptrn = Pattern<"?? 41 0F 5E ?? ?? 0F 5C ?? ?? 0F 11 87 ?? 4E 00 00">("Stamina");
    scanner.Add(stamina_ptrn, [this](PointerCalculator ptr) {
        PlayerStaminaPatch = BytePatches::Add(ptr.Add(5).As<std::uint8_t*>(), Nop<4>());
    });

    constexpr auto env_protection_ptrn = Pattern<"?? 0F 5E ?? 0F 28 ?? ?? 0F 58 4C BB 24">("EnvProtection");
    scanner.Add(env_protection_ptrn, [this](PointerCalculator ptr) {
        EnvProtectionPatch = BytePatches::Add(ptr.Add(7).As<std::uint8_t*>(), Nop<6>());
    });

    constexpr auto exosuit_shield_ptrn = Pattern<"FF 4F 18 ?? 0F 10 05 ?? ?? ?? ?? F3">("ExosuitShields");
    scanner.Add(exosuit_shield_ptrn, [this](PointerCalculator ptr) {
        ExosuitShieldsPatch = BytePatches::Add(ptr.As<uint8_t*>(), Nop<3>());
        /*
        * TODO: add a button to fill them instead
        * 48 8B ?? 48 85 ?? 74 ?? 48 63 89 60 01 00 00 48 8B 80 58 01 00 00
        FillExosuitShields = CavePatches::Add(ptr.As<uint8_t*>(), std::to_array<uint8_t>({
            0xC7, 0x47, 0x18, 0x32, 0x00, 0x00, 0x00,
            0x48, 0x8B, 0xC8,
            0x48, 0x85, 0xC0
        }));
        */
    });

    constexpr auto free_crafting_ptrn = Pattern<"44 29 7B 18 45 2B ??">("FreeCrafting");
    scanner.Add(free_crafting_ptrn, [this](PointerCalculator ptr) {
        FreeCraftingPatch = BytePatches::Add(ptr.As<std::uint8_t*>(), Nop<4>());
    });

    constexpr auto ship_shield_ptrn = Pattern<"44 0F 4D ?? 44 89 43 18 49 ?? 85 ?? ?? 00 00">("ShipShields");
    scanner.Add(ship_shield_ptrn, [this](PointerCalculator ptr) {
        StarshipShieldPatch = CavePatches::Add(ptr.As<uint8_t*>(), std::to_array<uint8_t>({
            0x44, 0x0F, 0x4D, 0xC0,
            0x41, 0x50,
            0x44, 0x8B, 0x43, 0x20,
            0x44, 0x89, 0x43, 0x18,
            0x41, 0x58
        }));
    });

    constexpr auto corvette_parts_ptrn = Pattern<"88 84 31 ?? ?? 00 00 EB ?? 49 63">("CorvetteParts");
    scanner.Add(corvette_parts_ptrn, [this](PointerCalculator ptr) {
        UnlockVettePtsPatch = CavePatches::Add(ptr.As<uint8_t*>(), std::to_array<uint8_t>({
            0xB0, 0x64, // mov al,64
            0x88, 0x84, 0x31, 0x58, 0x0F, 0x00, 0x00, // original code: mov [rcx+rsi+00000F58],al
        }));
    });

    constexpr auto launch_thruster_ptrn = Pattern<"2B ?? 48 8D 4C 24 78 89 47 18 48 8B">("LaunchThruster");
    scanner.Add(launch_thruster_ptrn, [this](PointerCalculator ptr) {
        LaunchThrusterPatch = BytePatches::Add(ptr.As<uint8_t*>(), Nop<2>());
    });

    constexpr auto pulse_engine_ptrn = Pattern<"0F 2F ?? 76 ?? 41 FF ?? 18">("PulseEngine");
    scanner.Add(pulse_engine_ptrn, [this](PointerCalculator ptr) {
        PulseEnginePatch = BytePatches::Add(ptr.Add(5).As<uint8_t*>(), Nop<4>());
    });

    constexpr auto hyper_drive_ptrn = Pattern<"2B ?? 85 ?? 41 0F 4E ?? 89 4B ??">("HyperDrive");
    scanner.Add(hyper_drive_ptrn, [this](PointerCalculator ptr) {
        HyperDrivePatch = BytePatches::Add(ptr.As<uint8_t*>(), Nop<2>());
    });

    constexpr auto ship_laser_ptrn = Pattern<"?? 0F 5E ?? ?? 41 0F 58 8C 86 ?? ?? ?? ??">("ShipLaser");
    scanner.Add(ship_laser_ptrn, [this](PointerCalculator ptr) {
        ShipLaserPatch = BytePatches::Add(ptr.Add(4).As<uint8_t*>(), Nop<10>());
    });

    constexpr auto rocket_cd_ptrn = Pattern<"41 89 84 8E ?? ?? ?? ?? F3 0F">("ShipRockets");
    scanner.Add(rocket_cd_ptrn, [this](PointerCalculator ptr) {
        RocketCooldownPatch = BytePatches::Add(ptr.Sub(7).As<std::uint8_t*>(),
            std::to_array<uint8_t>({ 0xB8, 0x00, 0x00, 0x80, 0x3F, 0x90, 0x90 }));
    });

    constexpr auto inf_mining_ptrn = Pattern<"41 29 45 18 01 1E B8 04 00 00 00 ??">("MiningBeam");
    scanner.Add(inf_mining_ptrn, [this](PointerCalculator ptr) {
        InfMiningBeamPatch = BytePatches::Add(ptr.As<std::uint8_t*>(), Nop<4>());
    });

    constexpr auto inf_mining_ptrn2 = Pattern<"41 0F 28 ?? 41 0F 28 ?? ?? 0F 58 86 ?? ?? ?? ?? F3">("MiningBeamHeat");
    scanner.Add(inf_mining_ptrn2, [this](PointerCalculator ptr) {
        InfMiningBeamPatch2 = BytePatches::Add(ptr.Add(8).As<std::uint8_t*>(), Nop<8>());
    });

    constexpr auto inf_terrain_ptrn = Pattern<"?? 0F 2C ?? 2B ?? 85 ?? 0F 4E ??">("TerrainManipulator");
    scanner.Add(inf_terrain_ptrn, [this](PointerCalculator ptr) {
        InfTerrainToolPatch = BytePatches::Add(ptr.Add(4).As<std::uint8_t*>(), Nop<2>());
    });

    constexpr auto inf_grenades_ptrn = Pattern<"33 ?? 48 8D 55 B0 48 8D 8D">("Grenades");
    scanner.Add(inf_grenades_ptrn, [this](PointerCalculator ptr) {
        InfGrenadesPatch = CavePatches::Add(ptr.As<std::uint8_t*>(), std::to_array<uint8_t>({
            0x41, 0x57,
            0x4C, 0x8B, 0x7B, 0x20, 0x49,
            0xFF, 0xC7, 0x4C, 0x89, 0x7B,
            0x18, 0x41, 0x5F,
            0x31, 0xF6,
            0x48, 0x8D, 0x55, 0xB0
        }));
    });

    constexpr auto inf_bc_ammo_ptrn = Pattern<"44 0F 4C ?? 41 2B ?? 41 89 84 B6 ?? ?? 00 00">("BoltCasterAmmo");
    scanner.Add(inf_bc_ammo_ptrn, [this](PointerCalculator ptr) {
        InfBcAmmoPatch = CavePatches::Add(ptr.As<std::uint8_t*>(), std::to_array<uint8_t>({
            0x44, 0x0F, 0x4C,
            0xF8, 0xB8, 0x40, 0x00, 0x00, 0x00
        }));
    });

    constexpr auto inf_bj_ammo_ptrn = Pattern<"89 46 18 48 8B 95 18 01 00 00">("BlazeJavelineAmmo");
    scanner.Add(inf_bj_ammo_ptrn, [this](PointerCalculator ptr) {
        InfBjAmmoPatch = CavePatches::Add(ptr.As<std::uint8_t*>(), std::to_array<uint8_t>({
            0x8B, 0x46, 0x20,
            0x89, 0x46, 0x18,
            0x48, 0x8B, 0x95, 0x18, 0x01, 0x00, 0x00
        }));
    });

    constexpr auto inf_nc_ammo_ptrn = Pattern<"89 46 18 49 8B 06 49 8B ??">("NeuronCannonAmmo");
    scanner.Add(inf_nc_ammo_ptrn, [this](PointerCalculator ptr) {
        InfNcAmmoPatch = CavePatches::Add(ptr.As<std::uint8_t*>(), std::to_array<uint8_t>({
            0x8B, 0x46, 0x20,
            0x89, 0x46, 0x18,
            0x49, 0x8B, 0x06
        }));
    });

    constexpr auto mtt_dmg_ptrn = Pattern<"89 7B 50 ?? 43 48 01 48 8B 4E">("MultitoolDamage");
    scanner.Add(mtt_dmg_ptrn, [this](PointerCalculator ptr) {
        OneHitKillsPatch = CavePatches::Add(ptr.As<std::uint8_t*>(), std::to_array<uint8_t>({
            0xC7, 0x43, 0x50,
            0xCD, 0xCC, 0xCC, 0x3D,
            0xC6, 0x43, 0x48, 0x01
        }));
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
