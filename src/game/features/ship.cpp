#include "ship.hpp"
#include "memory/pointers.hpp"


void Ship::Draw()
{
	ImGui::Spacing();

	Gui::PatchToggle("Infinite Shields", InfiniteShiels, g_Pointers.StarshipShieldPatch);

	Gui::PatchToggle("Infinite Launch Thrusters", InfiniteLaunchThrusters, g_Pointers.LaunchThrusterPatch);

	Gui::PatchToggle("Infinite Pulse Engine", InfinitePulseEngine, g_Pointers.PulseEnginePatch);

	Gui::PatchToggle("Infinite Hyperdrive", InfiniteHyperDrive, g_Pointers.HyperDrivePatch);

	Gui::PatchToggle("No Weapon Overheat",
		NoWeaponOverheat,
	    g_Pointers.ShipLaserPatch,
	    g_Pointers.RocketCooldownPatch
	);

	Gui::PatchToggle("Unlock All Corvette Parts", UnlockAllCorvetteParts, g_Pointers.UnlockVettePtsPatch);
}
