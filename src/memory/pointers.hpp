#pragma once
#include <vulkan/vulkan.h>
#include <windows.h>
#include "game/classes/currency.hpp"
#include "byte_patches.hpp"
#include "cave_patches.hpp"


struct PointerData
{
	// Vulkan
	PVOID QueuePresentKHR;
	PVOID CreateSwapchainKHR;
	PVOID AcquireNextImageKHR;
	PVOID AcquireNextImage2KHR;
	VkDevice* VkDevicePtr;
	HWND Hwnd;
	WNDPROC WndProc;

	// Game
	Currency* PlayerCurrency;
	void* CurrencyInstruction; // mov ecx, [rax+0000B8C0]
	float* GroundSpeed; // movss xmm6,[NMS.exe+4D74068]
	BytePatch PlayerHealthPatch;
	BytePatch PlayerStaminaPatch;
	BytePatch JetPackFuelPatch;
	BytePatch JetPackFuelPatch2;
	BytePatch LifeSupportPatch;
	BytePatch EnvProtectionPatch;
	BytePatch LaunchThrusterPatch;
	BytePatch PulseEnginePatch;
	BytePatch HyperDrivePatch;
	BytePatch ShipLaserPatch;
	BytePatch RocketCooldownPatch;
	BytePatch InfMiningBeamPatch;
	BytePatch InfMiningBeamPatch2;
	BytePatch InfTerrainToolPatch;
	BytePatch ExosuitShieldsPatch;
	BytePatch FreeCraftingPatch;
	CavePatch InfBcAmmoPatch;
	CavePatch InfBjAmmoPatch; // Blaze Javeline, not Blowjob
	CavePatch InfNcAmmoPatch;
	CavePatch InfGrenadesPatch;
	CavePatch FillExosuitShields;
	CavePatch StarshipShieldPatch;
	CavePatch UnlockVettePtsPatch; // corvette parts
	CavePatch OneHitKillsPatch;
};

struct Pointers : PointerData
{
	bool Init();
	bool LateInit();
	void Restore();
};

inline Pointers g_Pointers;
