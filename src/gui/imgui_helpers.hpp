#pragma once

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
