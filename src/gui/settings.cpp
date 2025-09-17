#include "settings.hpp"


void Settings::Draw()
{
	// TODO: refactor into separate tabs if this ends up having actual settings
	ImGui::Spacing();
	ImGui::SeparatorText("Style Editor");
	ImGui::Spacing();
	ImGui::ShowStyleEditor();
}
