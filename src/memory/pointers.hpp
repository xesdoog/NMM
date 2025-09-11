#pragma once
#include <vulkan/vulkan.h>
#include <windows.h>
#include "game/classes/currency.hpp"


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
	Currency* PlayerCurrency;
};

struct Pointers : PointerData
{
	bool Init();
	bool LateInit();
	void Restore();
};

inline Pointers g_pointers;
