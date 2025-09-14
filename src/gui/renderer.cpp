#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_win32.h>
#include <imgui.h>
#include <memory/pointers.hpp>
#include <windows.h>
#include <tlhelp32.h>
#include <common.hpp>
#include "renderer.hpp"
#include "gui.hpp"
#include "hooks/hooks.hpp"
#include "logging/logger.hpp"
#include "fonts/Fonts.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS


static HWND GetProcessMainWindow()
{
	DWORD currentPID = GetCurrentProcessId();
	HWND hwnd = nullptr;

	struct EnumData {
		DWORD pid;
		HWND hwnd;
	} data = { currentPID, nullptr };

	auto EnumWindowsProc = [](HWND hWnd, LPARAM lParam) -> BOOL {
		EnumData* pData = reinterpret_cast<EnumData*>(lParam);
		DWORD wndPID = 0;
		GetWindowThreadProcessId(hWnd, &wndPID);

		if (wndPID == pData->pid && IsWindowVisible(hWnd) && GetWindow(hWnd, GW_OWNER) == nullptr) {
			pData->hwnd = hWnd;
			return FALSE;
		}
		return TRUE;
		};

	EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&data));
	return data.hwnd;
}

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

void Renderer::DestroyImpl()
{
	GUI::Close();
	SetWindowLongPtr(g_pointers.Hwnd, GWLP_WNDPROC, (LONG_PTR)g_pointers.WndProc);
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	if (m_VkDevice && m_VkDescriptorPool) {
		vkDestroyDescriptorPool(m_VkDevice, m_VkDescriptorPool, m_VkAllocator);
		m_VkDescriptorPool = VK_NULL_HANDLE;
	}

	VkCleanupRenderTarget();
}

bool Renderer::InitImpl()
{
	g_pointers.Hwnd = GetProcessMainWindow();
	if (!g_pointers.Hwnd) {
		Logger::Log(ERR, "Failed to get window handle!");
		return false;
	}

	g_pointers.WndProc = (WNDPROC)GetWindowLongPtr(g_pointers.Hwnd, GWLP_WNDPROC);
	if (!g_pointers.WndProc) {
		Logger::Log(ERR, "Failed to get window procedure!");
		return false;
	}
	SetWindowLongPtr(g_pointers.Hwnd, GWLP_WNDPROC, (LONG_PTR)&Hooks::Window::WndProc);

	VkInstanceCreateInfo CreateInfo = {};
	const std::vector<const char*> InstanceExtensions = { "VK_KHR_surface" };
	const std::vector<const char*> ValidationLayers = { "VK_LAYER_KHRONOS_validation" };

	CreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	CreateInfo.enabledExtensionCount = (uint32_t)InstanceExtensions.size();
	CreateInfo.ppEnabledExtensionNames = InstanceExtensions.data();

	if (const VkResult result = vkCreateInstance(&CreateInfo, m_VkAllocator, &m_VkInstance); result != VK_SUCCESS)
	{
		Logger::Log(WARN, "vkCreateInstance failed");
		return false;
	};

	uint32_t GpuCount{};

	if (const VkResult result = vkEnumeratePhysicalDevices(m_VkInstance, &GpuCount, NULL); result != VK_SUCCESS)
	{
		Logger::Log(WARN, "vkEnumeratePhysicalDevices failed");
		return false;
	}

	IM_ASSERT(GpuCount > 0);

	ImVector<VkPhysicalDevice> GpuArr;
	GpuArr.resize(GpuCount);

	if (const VkResult result = vkEnumeratePhysicalDevices(m_VkInstance, &GpuCount, GpuArr.Data); result != VK_SUCCESS)
	{
		Logger::Log(WARN, "vkEnumeratePhysicalDevices 2 failed");
		return false;
	}

	VkPhysicalDevice MainGPU = nullptr;
	for (const auto& Gpu : GpuArr)
	{
		VkPhysicalDeviceProperties Properties;
		vkGetPhysicalDeviceProperties(Gpu, &Properties);
		if (Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			MainGPU = Gpu;
			break;
		}
	}

	if (!MainGPU)
	{
		Logger::Log(INFO, "Failed to get main GPU!");
		return false;
	}

	m_VkPhysicalDevice = MainGPU;

	uint32_t Count;
	vkGetPhysicalDeviceQueueFamilyProperties(m_VkPhysicalDevice, &Count, NULL);
	m_VKQueueFamilies.resize(Count);
	vkGetPhysicalDeviceQueueFamilyProperties(m_VkPhysicalDevice, &Count, m_VKQueueFamilies.data());

	for (uint32_t i = 0; i < Count; ++i)
	{
		if (m_VKQueueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			m_VkQueueFamily = i;
			break;
		}
	}

	IM_ASSERT(m_VkQueueFamily != (uint32_t)-1);

	constexpr const char* DeviceExtension = "VK_KHR_swapchain";
	constexpr const float QueuePriority = 1.0f;

	VkDeviceQueueCreateInfo DeviceQueueInfo = {};
	DeviceQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	DeviceQueueInfo.queueFamilyIndex = m_VkQueueFamily;
	DeviceQueueInfo.queueCount = 1;
	DeviceQueueInfo.pQueuePriorities = &QueuePriority;

	VkDeviceCreateInfo DeviceCreateInfo = {};
	DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	DeviceCreateInfo.queueCreateInfoCount = 1;
	DeviceCreateInfo.pQueueCreateInfos = &DeviceQueueInfo;
	DeviceCreateInfo.enabledExtensionCount = 1;
	DeviceCreateInfo.ppEnabledExtensionNames = &DeviceExtension;

	if (const VkResult result = vkCreateDevice(m_VkPhysicalDevice, &DeviceCreateInfo, m_VkAllocator, &m_VkFakeDevice); result != VK_SUCCESS)
	{
		Logger::Log(WARN, "Fake vkCreateDevice failed");
		return false;
	}

	g_pointers.QueuePresentKHR = reinterpret_cast<void*>(vkGetDeviceProcAddr(m_VkFakeDevice, "vkQueuePresentKHR"));
	g_pointers.CreateSwapchainKHR = reinterpret_cast<void*>(vkGetDeviceProcAddr(m_VkFakeDevice, "vkCreateSwapchainKHR"));
	g_pointers.AcquireNextImageKHR = reinterpret_cast<void*>(vkGetDeviceProcAddr(m_VkFakeDevice, "vkAcquireNextImageKHR"));
	g_pointers.AcquireNextImage2KHR = reinterpret_cast<void*>(vkGetDeviceProcAddr(m_VkFakeDevice, "vkAcquireNextImage2KHR"));

	Logger::Log(INFO, std::format("QueuePresentKHR export: 0x{:X}", (uintptr_t)g_pointers.QueuePresentKHR));
	Logger::Log(INFO, std::format("CreateSwapchainKHR export: 0x{:X}", (uintptr_t)g_pointers.CreateSwapchainKHR));
	Logger::Log(INFO, std::format("AcquireNextImageKHR export: 0x{:X}", (uintptr_t)g_pointers.AcquireNextImageKHR));
	Logger::Log(INFO, std::format("AcquireNextImage2KHR export: 0x{:X}", (uintptr_t)g_pointers.AcquireNextImage2KHR));

	vkDestroyDevice(m_VkFakeDevice, m_VkAllocator);
	m_VkFakeDevice = NULL;

	ImGui::CreateContext();
	Fonts::Load();
	ImGui_ImplWin32_Init(g_pointers.Hwnd);
	Logger::Log(INFO, "Vulkan renderer has finished initializing.");

	return true;
}

void Renderer::VkCreateRenderTarget(VkDevice device, VkSwapchainKHR swapchain)
{
	uint32_t uImageCount;
	if (const VkResult result = vkGetSwapchainImagesKHR(device, swapchain, &uImageCount, NULL))
	{
		Logger::Log(WARN, "vkGetSwapchainImagesKHR failed");
		return;
	}

	VkImage BackBuffers[8] = {};
	if (const VkResult result = vkGetSwapchainImagesKHR(device, swapchain, &uImageCount, BackBuffers))
	{
		Logger::Log(WARN, "vkGetSwapchainImagesKHR 2 failed");
		return;
	}

	for (uint32_t i = 0; i < uImageCount; ++i)
	{
		m_VkFrames[i].Backbuffer = BackBuffers[i];

		ImGui_ImplVulkanH_Frame* fd = &m_VkFrames[i];
		ImGui_ImplVulkanH_FrameSemaphores* fsd = &m_VkFrameSemaphores[i];
		{
			VkCommandPoolCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			info.queueFamilyIndex = m_VkQueueFamily;

			if (const VkResult result = vkCreateCommandPool(device, &info, m_VkAllocator, &fd->CommandPool))
			{
				Logger::Log(WARN, "vkCreateCommandPool failed");
				return;
			}
		}
		{
			VkCommandBufferAllocateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			info.commandPool = fd->CommandPool;
			info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			info.commandBufferCount = 1;

			if (const VkResult result = vkAllocateCommandBuffers(device, &info, &fd->CommandBuffer))
			{
				Logger::Log(WARN, "vkAllocateCommandBuffers failed");
				return;
			}
		}
		{
			VkFenceCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
			if (const VkResult result = vkCreateFence(device, &info, m_VkAllocator, &fd->Fence))
			{
				Logger::Log(WARN, "vkCreateFence failed");
				return;
			}
		}
		{
			VkSemaphoreCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			if (const VkResult result = vkCreateSemaphore(device, &info, m_VkAllocator, &fsd->ImageAcquiredSemaphore))
			{
				Logger::Log(WARN, "vkCreateSemaphore failed");
				return;
			}

			if (const VkResult result = vkCreateSemaphore(device, &info, m_VkAllocator, &fsd->RenderCompleteSemaphore))
			{
				Logger::Log(WARN, "vkCreateSemaphore 2 failed");
				return;
			}
		}
	}

	{
		VkAttachmentDescription attachment = {};
		attachment.format = VK_FORMAT_B8G8R8A8_UNORM;
		attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference color_attachment = {};
		color_attachment.attachment = 0;
		color_attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &color_attachment;

		VkRenderPassCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		info.attachmentCount = 1;
		info.pAttachments = &attachment;
		info.subpassCount = 1;
		info.pSubpasses = &subpass;

		if (const VkResult result = vkCreateRenderPass(device, &info, m_VkAllocator, &m_VkRenderPass))
		{
			Logger::Log(WARN, "vkCreateRenderPass failed");
			return;
		}
	}
	{
		VkImageViewCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		info.format = VK_FORMAT_B8G8R8A8_UNORM;

		info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		info.subresourceRange.baseMipLevel = 0;
		info.subresourceRange.levelCount = 1;
		info.subresourceRange.baseArrayLayer = 0;
		info.subresourceRange.layerCount = 1;

		for (uint32_t i = 0; i < uImageCount; ++i)
		{
			ImGui_ImplVulkanH_Frame* fd = &m_VkFrames[i];
			info.image = fd->Backbuffer;

			if (const VkResult result = vkCreateImageView(device, &info, m_VkAllocator, &fd->BackbufferView))
			{
				Logger::Log(WARN, "vkCreateImageView failed");
				return;
			}
		}
	}
	{
		VkImageView attachment[1];
		VkFramebufferCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		info.renderPass = m_VkRenderPass;
		info.attachmentCount = 1;
		info.pAttachments = attachment;
		info.layers = 1;

		for (uint32_t i = 0; i < uImageCount; ++i)
		{
			ImGui_ImplVulkanH_Frame* fd = &m_VkFrames[i];
			attachment[0] = fd->BackbufferView;

			if (const VkResult result = vkCreateFramebuffer(device, &info, m_VkAllocator, &fd->Framebuffer))
			{
				Logger::Log(WARN, "vkCreateFramebuffer failed");
				return;
			}
		}
	}

	if (!m_VkDescriptorPool)
	{
		constexpr VkDescriptorPoolSize pool_sizes[] = { {VK_DESCRIPTOR_TYPE_SAMPLER, 1000}, {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000}, {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000}, {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000}, {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000}, {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000}, {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000}, {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000}, {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000}, {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000}, {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000} };

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
		pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;

		if (const VkResult result = vkCreateDescriptorPool(device, &pool_info, m_VkAllocator, &m_VkDescriptorPool))
		{
			Logger::Log(WARN, "vkCreateDescriptorPool failed");
			return;
		}
	}
}

void Renderer::VkCleanupRenderTarget()
{
	for (uint32_t i = 0; i < RTL_NUMBER_OF(GetInstance().m_VkFrames); ++i)
	{
		if (GetInstance().m_VkFrames[i].Fence)
		{
			vkDestroyFence(GetInstance().m_VkDevice, GetInstance().m_VkFrames[i].Fence, GetInstance().m_VkAllocator);
			GetInstance().m_VkFrames[i].Fence = VK_NULL_HANDLE;
		}
		if (GetInstance().m_VkFrames[i].CommandBuffer)
		{
			vkFreeCommandBuffers(GetInstance().m_VkDevice,
				GetInstance().m_VkFrames[i].CommandPool,
				1,
				&GetInstance().m_VkFrames[i].CommandBuffer);
			GetInstance().m_VkFrames[i].CommandBuffer = VK_NULL_HANDLE;
		}
		if (GetInstance().m_VkFrames[i].CommandPool)
		{
			vkDestroyCommandPool(GetInstance().m_VkDevice, GetInstance().m_VkFrames[i].CommandPool, GetInstance().m_VkAllocator);
			GetInstance().m_VkFrames[i].CommandPool = VK_NULL_HANDLE;
		}
		if (GetInstance().m_VkFrames[i].BackbufferView)
		{
			vkDestroyImageView(GetInstance().m_VkDevice, GetInstance().m_VkFrames[i].BackbufferView, GetInstance().m_VkAllocator);
			GetInstance().m_VkFrames[i].BackbufferView = VK_NULL_HANDLE;
		}
		if (GetInstance().m_VkFrames[i].Framebuffer)
		{
			vkDestroyFramebuffer(GetInstance().m_VkDevice, GetInstance().m_VkFrames[i].Framebuffer, GetInstance().m_VkAllocator);
			GetInstance().m_VkFrames[i].Framebuffer = VK_NULL_HANDLE;
		}
	}

	for (uint32_t i = 0; i < RTL_NUMBER_OF(GetInstance().m_VkFrameSemaphores); ++i)
	{
		if (GetInstance().m_VkFrameSemaphores[i].ImageAcquiredSemaphore)
		{
			vkDestroySemaphore(GetInstance().m_VkDevice,
				GetInstance().m_VkFrameSemaphores[i].ImageAcquiredSemaphore,
				GetInstance().m_VkAllocator);
			GetInstance().m_VkFrameSemaphores[i].ImageAcquiredSemaphore = VK_NULL_HANDLE;
		}
		if (GetInstance().m_VkFrameSemaphores[i].RenderCompleteSemaphore)
		{
			vkDestroySemaphore(GetInstance().m_VkDevice,
				GetInstance().m_VkFrameSemaphores[i].RenderCompleteSemaphore,
				GetInstance().m_VkAllocator
			);

			GetInstance().m_VkFrameSemaphores[i].RenderCompleteSemaphore = VK_NULL_HANDLE;
		}
	}
}

bool Renderer::DoesQueueSupportGraphic(VkQueue queue, VkQueue* pGraphicQueue)
{
	for (uint32_t i = 0; i < m_VKQueueFamilies.size(); ++i)
	{
		const VkQueueFamilyProperties& family = m_VKQueueFamilies[i];
		for (uint32_t j = 0; j < family.queueCount; ++j)
		{
			VkQueue it = VK_NULL_HANDLE;
			vkGetDeviceQueue(m_VkDevice, i, j, &it);

			if (pGraphicQueue && family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				if (*pGraphicQueue == VK_NULL_HANDLE)
				{
					*pGraphicQueue = it;
				}
			}


			if (queue == it && family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				return true;
			}
		}
	}

	return false;
}

void Renderer::VkOnPresentImpl(VkQueue queue, const VkPresentInfoKHR* pPresentInfo)
{
    if (!m_VkDevice)
    {
        Logger::Log(ERR, "Invalid VkDevice");
        return;
    }

    if (!ImGui::GetCurrentContext())
    {
        ImGui::CreateContext(&GetInstance().m_FontAtlas);
        ImGui_ImplWin32_Init(g_pointers.Hwnd);
    }

    VkQueue GraphicQueue = VK_NULL_HANDLE;
    const bool QueueSupportsGraphic = DoesQueueSupportGraphic(queue, &GraphicQueue);

    for (uint32_t i = 0; i < pPresentInfo->swapchainCount; ++i)
    {
        VkSwapchainKHR swapchain = pPresentInfo->pSwapchains[i];
        if (m_VkFrames[0].Framebuffer == VK_NULL_HANDLE)
        {
            VkCreateRenderTarget(m_VkDevice, swapchain);

            if (!ImGui::GetIO().BackendRendererUserData)
            {
                ImGui_ImplVulkan_InitInfo init_info = {};
                init_info.Instance = m_VkInstance;
                init_info.PhysicalDevice = m_VkPhysicalDevice;
                init_info.Device = m_VkDevice;
                init_info.QueueFamily = m_VkQueueFamily;
                vkGetDeviceQueue(m_VkDevice, m_VkQueueFamily, 0, &GraphicQueue);
                init_info.Queue = GraphicQueue;
                init_info.PipelineCache = m_VkPipelineCache;
                init_info.DescriptorPool = m_VkDescriptorPool;
                init_info.Subpass = 0;
                init_info.MinImageCount = m_VkMinImageCount;
                init_info.ImageCount = m_VkMinImageCount;
                init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
                init_info.Allocator = m_VkAllocator;
                init_info.CheckVkResultFn = nullptr;
                init_info.RenderPass = m_VkRenderPass;
                ImGui_ImplVulkan_Init(&init_info);
            }
        }

        ImGui_ImplVulkanH_Frame* fd = &m_VkFrames[pPresentInfo->pImageIndices[i]];
        ImGui_ImplVulkanH_FrameSemaphores* fsd = &m_VkFrameSemaphores[pPresentInfo->pImageIndices[i]];
        {
            if (const VkResult result = vkWaitForFences(m_VkDevice, 1, &fd->Fence, VK_TRUE, ~0ull); result != VK_SUCCESS)
            {
                Logger::Log(WARN, "vkWaitForFences failed");
                return;
            }

            if (const VkResult result = vkResetFences(m_VkDevice, 1, &fd->Fence); result != VK_SUCCESS)
            {
                Logger::Log(WARN, "vkResetFences failed");
                return;
            }
        }
        {
            if (const VkResult result = vkResetCommandBuffer(fd->CommandBuffer, 0); result != VK_SUCCESS)
            {
                Logger::Log(WARN, "vkResetCommandBuffer failed");
                return;
            }

            VkCommandBufferBeginInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

            if (const VkResult result = vkBeginCommandBuffer(fd->CommandBuffer, &info); result != VK_SUCCESS)
            {
                Logger::Log(WARN, "vkBeginCommandBuffer failed");
                return;
            }
        }
        {
            VkRenderPassBeginInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            info.renderPass = m_VkRenderPass;
            info.framebuffer = fd->Framebuffer;

            if (m_VkImageExtent.width == 0 || m_VkImageExtent.height == 0)
            {
                info.renderArea.extent.width = 1920;
                info.renderArea.extent.height = 1080;
            }
            else
            {
                info.renderArea.extent = m_VkImageExtent;
            }

            vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
        }

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

		for (const auto& callback : m_RendererCallBacks | std::views::values)
			callback();

        ImGui::Render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), fd->CommandBuffer);
        vkCmdEndRenderPass(fd->CommandBuffer);
        vkEndCommandBuffer(fd->CommandBuffer);

        uint32_t waitSemaphoresCount = i == 0 ? pPresentInfo->waitSemaphoreCount : 0;
        if (waitSemaphoresCount == 0 && !QueueSupportsGraphic)
        {
            constexpr VkPipelineStageFlags stages_wait = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
            {
                VkSubmitInfo info = {};
                info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

                info.pWaitDstStageMask = &stages_wait;

                info.signalSemaphoreCount = 1;
                info.pSignalSemaphores = &fsd->RenderCompleteSemaphore;

                if (const VkResult result = vkQueueSubmit(queue, 1, &info, VK_NULL_HANDLE); result != VK_SUCCESS)
                {
                    Logger::Log(WARN, "vkQueueSubmit failed");
                    return;
                }
            }
            {
                VkSubmitInfo info = {};
                info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                info.commandBufferCount = 1;
                info.pCommandBuffers = &fd->CommandBuffer;

                info.pWaitDstStageMask = &stages_wait;
                info.waitSemaphoreCount = 1;
                info.pWaitSemaphores = &fsd->RenderCompleteSemaphore;

                info.signalSemaphoreCount = 1;
                info.pSignalSemaphores = &fsd->ImageAcquiredSemaphore;

                if (const VkResult result = vkQueueSubmit(GraphicQueue, 1, &info, fd->Fence); result != VK_SUCCESS)
                {
                    Logger::Log(WARN, "vkQueueSubmit 2 failed");
                    return;
                }
            }
        }
        else
        {
            std::vector<VkPipelineStageFlags> stages_wait(waitSemaphoresCount, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

            VkSubmitInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            info.commandBufferCount = 1;
            info.pCommandBuffers = &fd->CommandBuffer;

            info.pWaitDstStageMask = stages_wait.data();
            info.waitSemaphoreCount = waitSemaphoresCount;
            info.pWaitSemaphores = pPresentInfo->pWaitSemaphores;

            info.signalSemaphoreCount = 1;
            info.pSignalSemaphores = &fsd->ImageAcquiredSemaphore;

            if (const VkResult result = vkQueueSubmit(GraphicQueue, 1, &info, fd->Fence); result != VK_SUCCESS)
            {
                Logger::Log(WARN, "vkQueueSubmit 3 failed");
                return;
            }
        }
    }
}

bool Renderer::AddRendererCallBackImpl(RendererCallBack&& callback, std::uint32_t priority)
{
	return m_RendererCallBacks.insert({ priority, callback }).second;
}

void Renderer::AddWindowProcedureCallbackImpl(WindowProcedureCallback&& callback)
{
	return m_WindowProcedureCallbacks.push_back(callback);
}

LRESULT Renderer::WndProcImpl(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	for (const auto& callback : m_WindowProcedureCallbacks)
		callback(hwnd, msg, wparam, lparam);

	return ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);
}
