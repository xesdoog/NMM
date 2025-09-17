#include "ship.hpp"
#include "memory/pointers.hpp"


void Ship::ToggleWeaponsCooldown(bool toggle)
{
	if (!g_Pointers.ShipLaserPatch || !g_Pointers.RocketCooldownPatch)
		return;

	if (toggle)
	{
		g_Pointers.ShipLaserPatch->Apply();
		g_Pointers.RocketCooldownPatch->Apply();
	}
	else
	{
		g_Pointers.ShipLaserPatch->Restore();
		g_Pointers.RocketCooldownPatch->Restore();
	}
}

void Ship::Draw()
{
	ImGui::Spacing();

	if (ImGui::Checkbox("Infinite Shields", &InfiniteShiels) && g_Pointers.StarshipShieldPatch)
		InfiniteShiels ? g_Pointers.StarshipShieldPatch->Apply() : g_Pointers.StarshipShieldPatch->Restore();

	if (ImGui::Checkbox("Infinite Launch Thrusters", &InfiniteLaunchThrusters) && g_Pointers.LaunchThrusterPatch)
		InfiniteLaunchThrusters ? g_Pointers.LaunchThrusterPatch->Apply() : g_Pointers.LaunchThrusterPatch->Restore();

	if (ImGui::Checkbox("Infinite Pulse Engine", &InfinitePulseEngine) && g_Pointers.PulseEnginePatch)
		InfinitePulseEngine ? g_Pointers.PulseEnginePatch->Apply() : g_Pointers.PulseEnginePatch->Restore();

	if (ImGui::Checkbox("Infinite Hyperdrive", &InfiniteHyperDrive) && g_Pointers.HyperDrivePatch)
		InfiniteHyperDrive ? g_Pointers.HyperDrivePatch->Apply() : g_Pointers.HyperDrivePatch->Restore();

	if (ImGui::Checkbox("No Weapon Overheat", &NoWeaponOverheat))
		ToggleWeaponsCooldown(&NoWeaponOverheat);
}
