#include "multitool.hpp"
#include "memory/pointers.hpp"


void Multitool::ToggleInfiniteAmmo(bool toggle)
{
	if (!g_Pointers.InfBcAmmoPatch && !g_Pointers.InfBjAmmoPatch && !g_Pointers.InfNcAmmoPatch)
		return;

	if (toggle)
	{
		g_Pointers.InfBcAmmoPatch->Apply();
		g_Pointers.InfBjAmmoPatch->Apply();
		g_Pointers.InfNcAmmoPatch->Apply();
	}
	else
	{
		g_Pointers.InfBcAmmoPatch->Restore();
		g_Pointers.InfBjAmmoPatch->Restore();
		g_Pointers.InfNcAmmoPatch->Restore();
	}
}

void Multitool::Draw()
{
	if (ImGui::Checkbox("Infinite Mining Beam", &InfiniteMiningBeam) && g_Pointers.InfMiningBeamPatch)
		InfiniteMiningBeam ? g_Pointers.InfMiningBeamPatch->Apply() : g_Pointers.InfMiningBeamPatch->Restore();

	if (ImGui::Checkbox("No Mining Beam Overheat", &NoMiningBeamOverheat) && g_Pointers.InfMiningBeamPatch2)
		NoMiningBeamOverheat ? g_Pointers.InfMiningBeamPatch2->Apply() : g_Pointers.InfMiningBeamPatch2->Restore();

	if (ImGui::Checkbox("Infinite Terrain Manipulator", &InfiniteTerrainTool) && g_Pointers.InfTerrainToolPatch)
		InfiniteTerrainTool ? g_Pointers.InfTerrainToolPatch->Apply() : g_Pointers.InfTerrainToolPatch->Restore();

	if (ImGui::Checkbox("Infinite Ammo", &InfiniteAmmo))
		ToggleInfiniteAmmo(&InfiniteAmmo);

	if (ImGui::Checkbox("Infinite Grenades", &InfiniteGrenades) && g_Pointers.InfGrenadesPatch)
		InfiniteGrenades ? g_Pointers.InfGrenadesPatch->Apply() : g_Pointers.InfGrenadesPatch->Restore();

	if (ImGui::Checkbox("One Hit Kill", &OneHitKills) && g_Pointers.OneHitKillsPatch)
		OneHitKills ? g_Pointers.OneHitKillsPatch->Apply() : g_Pointers.OneHitKillsPatch->Restore();
}
