#pragma once
#include <vulkan/vulkan.h>
#include <windows.h>
#include "../game/classes/interloper.hpp"
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
	GcPlayer* Interloper;
	void* CurrencyFunc;
	Currency* PlayerCurrency;
};

struct Pointers : PointerData
{
	bool Init();
	bool LateInit();
	void Restore();
};

inline Pointers g_pointers;
