#pragma once
#include <vulkan/vulkan.h>
#include <backends/imgui_impl_vulkan.h>
#include <functional>
#include <map>
#include <windows.h>
#include <wrl/client.h>
#include "logging/logger.hpp"


#define REL(o)       \
	o->Release();    \
	if (o)           \
	{                \
		o = nullptr; \
	}

using namespace Microsoft::WRL;
using RendererCallBack = std::function<void()>;
using WindowProcedureCallback = std::function<void(HWND, UINT, WPARAM, LPARAM)>;

class Renderer final
{
private:
	Renderer();

public:
	~Renderer();

	Renderer(const Renderer&) = delete;
	Renderer(Renderer&&) noexcept = delete;
	Renderer& operator=(const Renderer&) = delete;
	Renderer& operator=(Renderer&&) noexcept = delete;

	static void Destroy()
	{
		GetInstance().DestroyImpl();
	}

	static bool Init()
	{
		return GetInstance().InitImpl();
	}

	/**
		* @brief Add a callback function to draw using ImGui
		*
		* @param callback Callback function
		* @param priority Low values will be drawn before higher values.
		* @return true Successfully added callback.
		* @return false Duplicate render priority was given.
		*/
	static bool AddRendererCallBack(RendererCallBack&& callback, std::uint32_t priority)
	{
		return GetInstance().AddRendererCallBackImpl(std::move(callback), priority);
	}
	/**
		* @brief Add a callback function to handle Windows WindowProcedure
		*
		* @param callback Callback function
		*/
	static void AddWindowProcedureCallback(WindowProcedureCallback&& callback)
	{
		GetInstance().AddWindowProcedureCallbackImpl(std::move(callback));
	}

	static void VkOnPresent(VkQueue queue, const VkPresentInfoKHR* pPresentInfo)
	{
		if (!queue)
		{
			Logger::Log(ERR, "Invalid Vulkan Queue!");

			return;
		}

		if (!pPresentInfo)
		{
			Logger::Log(ERR, "Invalid Vulkan Present Info!");

			return;
		}

		GetInstance().VkOnPresentImpl(queue, pPresentInfo);
	}

	static LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		return GetInstance().WndProcImpl(hwnd, msg, wparam, lparam);
	}

	static void VkCleanupRenderTarget();

	static void VkSetDevice(VkDevice device)
	{
		if (!device)
		{
			Logger::Log(ERR, "Invalid Vulkan Device!");

			return;
		}

		GetInstance().m_VkDevice = device;
	}
	static void VkSetScreenSize(VkExtent2D extent)
	{
		GetInstance().m_VkImageExtent = extent;
	}

	static VkDevice VkGetDevice()
	{
		return GetInstance().m_VkDevice;
	}

private:
	void DestroyImpl();
	bool InitImpl();
	void VkCreateRenderTarget(VkDevice Device, VkSwapchainKHR Swapchain);
	bool AddRendererCallBackImpl(RendererCallBack&& callback, std::uint32_t priority);
	void AddWindowProcedureCallbackImpl(WindowProcedureCallback&& callback);
	void VkOnPresentImpl(VkQueue queue, const VkPresentInfoKHR* pPresentInfo);
	bool DoesQueueSupportGraphic(VkQueue queue, VkQueue* pGraphicQueue);

	LRESULT WndProcImpl(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	static Renderer& GetInstance()
	{
		static Renderer i{};

		return i;
	}

private:
	bool m_Resizing;

	ImFontAtlas m_FontAtlas;
	VkPhysicalDevice m_VkPhysicalDevice;
	VkInstance m_VkInstance;
	VkAllocationCallbacks* m_VkAllocator = nullptr;
	std::vector<VkQueueFamilyProperties> m_VKQueueFamilies;
	uint32_t m_VkQueueFamily = (uint32_t)-1;
	VkDevice m_VkFakeDevice;
	VkDevice m_VkDevice;
	ImGui_ImplVulkanH_Frame m_VkFrames[8] = {};
	ImGui_ImplVulkanH_FrameSemaphores m_VkFrameSemaphores[8] = {};
	VkRenderPass m_VkRenderPass;
	VkDescriptorPool m_VkDescriptorPool;
	VkPipelineCache m_VkPipelineCache;
	uint32_t m_VkMinImageCount = 2;
	VkExtent2D m_VkImageExtent;

private:
	std::map<uint32_t, RendererCallBack> m_RendererCallBacks;
	std::vector<WindowProcedureCallback> m_WindowProcedureCallbacks;
};

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
