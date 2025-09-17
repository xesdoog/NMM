#include "multitool.hpp"
#include "memory/pointers.hpp"

void Multitool::Draw()
{
	if (ImGui::Checkbox("Infinite Multitool", &InfiniteMultitool) && g_Pointers.InfMultitoolPatch)
		InfiniteMultitool ? g_Pointers.InfMultitoolPatch->Apply() : g_Pointers.InfMultitoolPatch->Restore();

	if (ImGui::Checkbox("Infinite Mining Beam", &InfiniteMiningBeam) && g_Pointers.InfMiningBeamPatch)
		InfiniteMiningBeam ? g_Pointers.InfMiningBeamPatch->Apply() : g_Pointers.InfMiningBeamPatch->Restore();

	if (ImGui::Checkbox("Infinite Terrain Manipulator", &InfiniteTerrainTool) && g_Pointers.InfTerrainToolPatch)
		InfiniteTerrainTool ? g_Pointers.InfTerrainToolPatch->Apply() : g_Pointers.InfTerrainToolPatch->Restore();

	if (ImGui::Checkbox("One Hit Kill", &OneHitKills) && g_Pointers.OneHitKillsPatch)
		OneHitKills ? g_Pointers.OneHitKillsPatch->Apply() : g_Pointers.OneHitKillsPatch->Restore();
}
