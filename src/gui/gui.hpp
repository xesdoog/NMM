#pragma once
#include <common.hpp>
#include <vulkan/vulkan.h>

using GuiCallBack = std::function<void()>;

struct Tab {
	std::string m_name;
	GuiCallBack m_callback;
	std::optional<std::string> m_hint;
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

	static bool AddTab(const std::string& name, GuiCallBack&& callback, std::optional<std::string> hint)
	{
		return GetInstance().AddTabImpl(name, std::move(callback), hint);
	}

	static void Draw()
	{
		GetInstance().DrawImpl();
	}

	static void SetActiveTab(std::string name, GuiCallBack tabfunc, std::optional<std::string> hint) {
		auto& tab = GetInstance().m_ActiveTab;
		tab.m_name = name;
		tab.m_callback = tabfunc;

		if (hint.has_value())
			tab.m_hint = hint;
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
	bool AddTabImpl(const std::string& name, GuiCallBack&& callback, std::optional<std::string> hint);
	ImVec2 m_WindowSize;
	ImVec2 m_WindowPos = ImVec2(0.1f, 0.1f);
	std::vector<Tab> m_Tabs;
	Tab m_ActiveTab;

	static GUI& GetInstance()
	{
		static GUI i{};
		return i;
	}

	bool m_IsOpen = true;
};

