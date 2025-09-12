#pragma once
#include <common.hpp>
#include <vulkan/vulkan.h>

using GuiCallBack = std::function<void()>;

struct ActiveTab
{
	static inline std::string m_name;
	static inline GuiCallBack m_callback;
};

class GUI final
{
public:

	static void Init()
	{
		GetInstance().InitImpl();
	}

	static void Toggle()
	{
		GetInstance().m_IsOpen ^= true;
		GetInstance().OverrideMouse();
	}

	static bool IsOpen()
	{
		return GetInstance().m_IsOpen;
	}

	static bool IsUsingKeyboard()
	{
		return ImGui::GetIO().WantTextInput;
	}

	static bool AddTab(const std::string& name, GuiCallBack&& callback)
	{
		return GetInstance().AddTabImpl(name, std::move(callback));
	}

	static void Draw()
	{
		GetInstance().DrawImpl();
	}

	static void SetActiveTab(std::string name, GuiCallBack tabfunc) {
		auto& tab = GetInstance().m_ActiveTab;
		tab.m_name = name;
		tab.m_callback = tabfunc;
	}

	static void SetWindowSize(VkExtent2D gameWindowSize) {
		ImVec2 size = ImVec2((float)gameWindowSize.width * 0.35f, (float)gameWindowSize.height - 40.0f);
		GetInstance().m_WindowSize = size;
	}

	static void Close()
	{
		GetInstance().CloseImpl();
	}

private:
	void InitImpl();
	void OverrideMouse();
	void CloseImpl();
	void DrawImpl();
	bool AddTabImpl(const std::string& name, GuiCallBack&& callback);
	std::map<std::string, GuiCallBack> m_GuiCallbacks;
	ImVec2 m_WindowSize;
	ImVec2 m_WindowPos = ImVec2(0.1f, 0.1f);
	ActiveTab m_ActiveTab;

	static GUI& GetInstance()
	{
		static GUI i{};
		return i;
	}

	bool m_IsOpen = true;
};

