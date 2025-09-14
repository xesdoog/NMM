#pragma once
#include <vulkan/vulkan.h>
#include <windows.h>
#include "game/classes/currency.hpp"
#include "byte_patches.hpp"


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
	void* CurrencyInstruction; // mov ecx, [rax+0000B8C0]
	void* LifeSupportInstruction; // mov [rax+18],ecx
	Currency* PlayerCurrency;
	BytePatch PlayerStaminaPatch;
	BytePatch JetPackFuelPatch; // subss xmm0,xmm1
	BytePatch JetPackFuelPatch2; // subss xmm0,xmm1 maxss xmm8,xmm1
};

struct Pointers : PointerData
{
	bool Init();
	bool LateInit();
	void Restore();
};

inline Pointers g_pointers;
