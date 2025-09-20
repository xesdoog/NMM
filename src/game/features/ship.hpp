#pragma once

class Ship
{
public:
	static void Draw();
	static void ToggleWeaponsCooldown(bool toggle);

	static inline bool InfiniteShiels;
	static inline bool InfiniteLaunchThrusters;
	static inline bool InfinitePulseEngine;
	static inline bool InfiniteHyperDrive;
	static inline bool NoWeaponOverheat;
	static inline bool UnlockAllCorvetteParts;
};
