#include "hooking.hpp"
#include "base_hook.hpp"
#include "detour_hook.hpp"
#include "hooks/hooks.hpp"
#include "gui/renderer.hpp"
#include "memory/pointers.hpp"
#include "Logging/logger.hpp"


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
}

bool Hooking::InitImpl() 
{
	if (!g_pointers.QueuePresentKHR)
	{
		Logger::Log(ERR, "Failed to install Vulkan hooks: One or more pointers are null");
		return false;
	}

	// Vulkan
	BaseHook::Add<Hooks::Vulkan::QueuePresentKHR>(new DetourHook("Vulkan::QueuePresentKHR", g_pointers.QueuePresentKHR, Hooks::Vulkan::QueuePresentKHR));
	BaseHook::Add<Hooks::Vulkan::CreateSwapchainKHR>(new DetourHook("Vulkan::CreateSwapchainKHR", g_pointers.CreateSwapchainKHR, Hooks::Vulkan::CreateSwapchainKHR));
	BaseHook::Add<Hooks::Vulkan::AcquireNextImage2KHR>(new DetourHook("Vulkan::AcquireNextImage2KHR", g_pointers.AcquireNextImage2KHR, Hooks::Vulkan::AcquireNextImage2KHR));
	BaseHook::Add<Hooks::Vulkan::AcquireNextImageKHR>(new DetourHook("Vulkan::AcquireNextImageKHR", g_pointers.AcquireNextImageKHR, Hooks::Vulkan::AcquireNextImageKHR));

	// WndProc
	// BaseHook::Add<Hooks::Window::WndProc>(new DetourHook("Window::WndProc", g_pointers.WndProc, Hooks::Window::WndProc));

	// Game
	// BaseHook::Add<Hooks::Game::MoneyFunction>(new DetourHook("Game::MoneyFunction", g_pointers.CurrencyFunc, Hooks::Game::MoneyFunction));

	BaseHook::EnableAll();

	auto mh_status = m_MinHook.ApplyQueued();

	if (mh_status == MH_OK)
	{
		Logger::Log(INFO, "Hooks applied");
		return true;
	}
	else
	{
		Logger::Log(ERR, "Failed to apply hooks!");
		return false;
	}
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