#pragma once

class Multitool
{
public:
	static void Draw();
	static void ToggleInfiniteAmmo(bool toggle);

	static inline bool InfiniteMiningBeam;
	static inline bool NoMiningBeamOverheat;
	static inline bool InfiniteTerrainTool;
	static inline bool InfiniteAmmo;
	static inline bool InfiniteGrenades;
	inline static bool OneHitKills;
};
