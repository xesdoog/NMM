#include "hooks/detour_hook.hpp"
#include "hooks/hooks.hpp"
#include "gui/renderer.hpp"
#include "gui/gui.hpp"
#include "memory/pointers.hpp"
#include "game/features/self.hpp"


namespace Hooks
{
	VkResult VKAPI_CALL Vulkan::QueuePresentKHR(VkQueue queue, const VkPresentInfoKHR* pPresentInfo)
	{		
		Renderer::VkOnPresent(queue, pPresentInfo);
		return BaseHook::Get<Vulkan::QueuePresentKHR, DetourHook<decltype(&QueuePresentKHR)>>()->Original()(queue, pPresentInfo);
	}

	VkResult VKAPI_CALL Vulkan::CreateSwapchainKHR(
		VkDevice device,
		const VkSwapchainCreateInfoKHR* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkSwapchainKHR* pSwapchain)
	{
		// what the fuck
		if (pCreateInfo)
		{
			Renderer::VkSetDevice(device);
			Renderer::VkCleanupRenderTarget();
			Renderer::VkSetScreenSize(pCreateInfo->imageExtent);
		}

		return BaseHook::Get<Vulkan::CreateSwapchainKHR, DetourHook<decltype(&CreateSwapchainKHR)>>()->Original()(device, pCreateInfo, pAllocator, pSwapchain);
	}

	VkResult VKAPI_CALL Vulkan::AcquireNextImage2KHR(VkDevice device, const VkAcquireNextImageInfoKHR* pAcquireInfo, uint32_t* pImageIndex)
	{
		if (!Renderer::VkGetDevice())
		{
			Renderer::VkSetDevice(device);
		}
		else if (Renderer::VkGetDevice() != device)
		{
			Renderer::VkSetDevice(device);
			Renderer::VkCleanupRenderTarget();
		}

		return BaseHook::Get<Vulkan::AcquireNextImage2KHR, DetourHook<decltype(&AcquireNextImage2KHR)>>()->Original()(device, pAcquireInfo, pImageIndex);
	}

	VkResult VKAPI_CALL Vulkan::AcquireNextImageKHR(VkDevice device, VkSwapchainKHR swapchain, uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t* pImageIndex)
	{
		if (!Renderer::VkGetDevice())
		{
			Renderer::VkSetDevice(device);
		}
		else if (Renderer::VkGetDevice() != device)
		{
			Renderer::VkSetDevice(device);
			Renderer::VkCleanupRenderTarget();
		}

		return BaseHook::Get<Vulkan::AcquireNextImageKHR, DetourHook<decltype(&AcquireNextImageKHR)>>()->Original()(device, swapchain, timeout, semaphore, fence, pImageIndex);
	}

	LRESULT Window::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (GUI::IsOpen()) {
			ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

			// https://stackoverflow.com/a/44175942
			switch (uMsg)
			{
			case WM_LBUTTONDOWN:
				wParam -= 11141008;
				if (wParam != MK_LBUTTON)
					return 0;
				break;

			case WM_LBUTTONUP:
				wParam -= 11141008;
				if (wParam != 0)
					return 0;
				break;

			case WM_MOUSEHOVER:
			case WM_MOUSEMOVE:
				if (GUI::IsOpen())
					return 0;
			}
			return 1;
		}

		return CallWindowProc(g_Pointers.WndProc, hWnd, uMsg, wParam, lParam);
	}
}
