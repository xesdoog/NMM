#include "multitool.hpp"
#include "memory/pointers.hpp"


void Multitool::Draw()
{

	Gui::PatchToggle("Infinite Mining Beam", InfiniteMiningBeam, g_Pointers.InfMiningBeamPatch);

	Gui::PatchToggle("No Mining Beam Overheat", NoMiningBeamOverheat, g_Pointers.InfMiningBeamPatch2);

	Gui::PatchToggle("Infinite Terrain Manipulator", InfiniteTerrainTool, g_Pointers.InfTerrainToolPatch);

	Gui::PatchToggle("Infinite Ammo",
		InfiniteAmmo,
	    g_Pointers.InfBcAmmoPatch,
	    g_Pointers.InfBjAmmoPatch,
	    g_Pointers.InfNcAmmoPatch
	);

	Gui::PatchToggle("Infinite Grenades", InfiniteGrenades, g_Pointers.InfGrenadesPatch);

	Gui::PatchToggle("One Hit Kill", OneHitKills, g_Pointers.OneHitKillsPatch);
}
