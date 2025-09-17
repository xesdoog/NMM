#include "hooking.hpp"
#include "base_hook.hpp"
#include "detour_hook.hpp"
#include "inline_hook.hpp"
#include "hooks.hpp"
#include "memory/pointers.hpp"


Hooking::Hooking()
{
}

Hooking::~Hooking()
{
	DestroyImpl();
}

bool Hooking::Init() 
{ 
	return GetInstance().InitImpl(); 
}

void Hooking::Destroy()
{
	GetInstance().DestroyImpl();
	BytePatches::RestoreAll();
	CavePatches::RestoreAll();
}

bool Hooking::InitImpl() 
{
	if (!g_Pointers.QueuePresentKHR)
	{
		LOG(FATAL) << "Failed to install Vulkan hooks: One or more pointers are null";
		return false;
	}

	// Vulkan
	BaseHook::Add<Hooks::Vulkan::QueuePresentKHR>(new DetourHook("Vulkan::QueuePresentKHR", g_Pointers.QueuePresentKHR, Hooks::Vulkan::QueuePresentKHR));
	BaseHook::Add<Hooks::Vulkan::CreateSwapchainKHR>(new DetourHook("Vulkan::CreateSwapchainKHR", g_Pointers.CreateSwapchainKHR, Hooks::Vulkan::CreateSwapchainKHR));
	BaseHook::Add<Hooks::Vulkan::AcquireNextImage2KHR>(new DetourHook("Vulkan::AcquireNextImage2KHR", g_Pointers.AcquireNextImage2KHR, Hooks::Vulkan::AcquireNextImage2KHR));
	BaseHook::Add<Hooks::Vulkan::AcquireNextImageKHR>(new DetourHook("Vulkan::AcquireNextImageKHR", g_Pointers.AcquireNextImageKHR, Hooks::Vulkan::AcquireNextImageKHR));
	
	// Game
	BaseHook::Add<nullptr>(new InlineHook("CurrencyHook", g_Pointers.CurrencyInstruction, &g_Pointers.PlayerCurrency, 6));

	BaseHook::EnableAll();

	auto mh_status = m_MinHook.ApplyQueued();
	if (mh_status != MH_OK)
	{
		LOG(FATAL) << "Failed to apply hooks!";
		return false;
	}

	LOG(INFO) << "Hooks applied";
	return true;
}

void Hooking::DestroyImpl()
{
	BaseHook::DisableAll();
	m_MinHook.ApplyQueued();

	for (auto it : BaseHook::Hooks())
	{
		delete it;
	}

	BaseHook::Hooks().clear();
}
