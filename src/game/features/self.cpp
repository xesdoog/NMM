#include "self.hpp"
#include "gui/gui.hpp"
#include "util/format_money.hpp"

uint32_t Self::GetCurrency(uint32_t Currency::* field) {
	return g_pointers.PlayerCurrency ? g_pointers.PlayerCurrency->*field : 0;
}

void Self::SetCurrency(uint32_t Currency::* field, uint32_t value) {
	if (g_pointers.PlayerCurrency)
		g_pointers.PlayerCurrency->*field = value;
}

uint32_t Self::GetUnits() {
	return GetCurrency(&Currency::Units);
}

uint32_t Self::GetNanites() {
	return GetCurrency(&Currency::Nanites);
}

uint32_t Self::GetQuicksilver() {
	return GetCurrency(&Currency::Quicksilver);
}

void Self::SetUnits(uint32_t amount) {
	SetCurrency(&Currency::Units, amount);
}

void Self::SetNanites(uint32_t amount) {
	SetCurrency(&Currency::Nanites, amount);
}

void Self::SetQuicksilver(uint32_t amount) {
	SetCurrency(&Currency::Quicksilver, amount);
}

void Self::DrawMain()
{
	ImGui::Spacing();

	bool JPPClicked = ImGui::Checkbox("Infinite Jetpack", &InfiniteJetpack);
	if (JPPClicked)
	{
		if (InfiniteJetpack)
		{
			g_pointers.JetPackFuelPatch->Apply();
			g_pointers.JetPackFuelPatch2->Apply();
		}
		else
		{
			g_pointers.JetPackFuelPatch->Restore();
			g_pointers.JetPackFuelPatch2->Restore();
		}
	}

	bool InfStamClicked = ImGui::Checkbox("Infinite Stamina", &InfiniteStamina);
	if (InfStamClicked)
	{
		if (InfiniteStamina)
		{
			g_pointers.PlayerStaminaPatch->Apply();
		}
		else
		{
			g_pointers.PlayerStaminaPatch->Restore();
		}
	}

	// ImGui::Checkbox("Infinite Life Support", &InfiniteLifeSupport);

	ImGui::Spacing();
	ImGui::SeparatorText(ICON_FA_MONEY_BILL);
	ImGui::Spacing();

	if (!g_pointers.PlayerCurrency) {
		ImGui::TextColored(ImVec4(1.0f, 0.01f, 0.01f, 0.9f), "Currency data is not available! Try opening your inventory.");
		return;
	}

	uint32_t units = GetUnits();
	uint32_t nanites = GetNanites();
	uint32_t quicksilver = GetQuicksilver();
	uint32_t step = 1e3;
	uint32_t stepFast = 1e5;

	ImGui::SeparatorText("Units");
	ImGui::Text(FormatMoney(units).c_str());
	ImGui::Spacing();
	if (ImGui::InputScalar("##UnitsScalar", ImGuiDataType_U32, &units, &step, &stepFast))
		SetUnits(units);

	ImGui::SeparatorText("Nanites");
	ImGui::Text(FormatMoney(nanites).c_str());
	ImGui::Spacing();
	if (ImGui::InputScalar("##NanitesScalar", ImGuiDataType_U32, &nanites, &step, &stepFast))
		SetNanites(nanites);

	ImGui::SeparatorText("Quicksilver");
	ImGui::Text(FormatMoney(quicksilver).c_str());
	ImGui::Spacing();
	if (ImGui::InputScalar("##QuicksilverScalar", ImGuiDataType_U32, &quicksilver, &step, &stepFast))
		SetQuicksilver(quicksilver);
}

void Self::DrawResources()
{
	ImGui::Text("Placeholder");
}

void Self::Draw()
{
	if (ImGui::BeginTabBar("selfTabBar"))
	{
		if (ImGui::BeginTabItem("Main"))
		{
			DrawMain();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Resources"))
		{
			DrawResources();
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}
