#pragma once

namespace Gui
{
	static void Tooltip(const char* text)
	{
		if (!ImGui::IsItemHovered())
			return;

		ImGui::SetNextWindowBgAlpha(0.8f);
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 15);
		ImGui::Text(text);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}

	static void HelpMarker(const char* text)
	{
		ImGui::SameLine();
		ImGui::TextDisabled("(?)");
		Tooltip(text);
	}

	template<typename... TPatches>
	bool PatchToggle(const char* label, bool& toggle, TPatches&&... patches)
	{
		if (ImGui::Checkbox(label, &toggle))
		{
			(void)std::initializer_list<int>{(patches ? (toggle ? patches->Apply() : patches->Restore(), 0) : 0)...};
			return true;
		}
		return false;
	}
}
