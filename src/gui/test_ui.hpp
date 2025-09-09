#pragma once

class GUI final
{
public:
	static void Init() {
		GetInstance().InitImpl();
	};

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

private:
	static void InitImpl();
	static void OverrideMouse();

	static GUI& GetInstance()
	{
		static GUI i{};
		return i;
	}

	bool m_IsOpen = true;
};
