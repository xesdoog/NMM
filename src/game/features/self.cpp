#include "self.hpp"
#include "memory/pointers.hpp"
#include "util/format_money.hpp"


uint32_t CurrencyStep = 1e3;
uint32_t CurrencyStepFast = 1e5;

uint32_t Self::GetCurrency(uint32_t Currency::* field) {
	return g_Pointers.PlayerCurrency ? g_Pointers.PlayerCurrency->*field : 0;
}

void Self::SetCurrency(uint32_t Currency::* field, uint32_t value) {
	if (g_Pointers.PlayerCurrency)
		g_Pointers.PlayerCurrency->*field = value;
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

void Self::ToggleInfiniteJetpack(bool toggle)
{
	if (!g_Pointers.JetPackFuelPatch || !g_Pointers.JetPackFuelPatch2)
		return;

	if (toggle)
	{
		g_Pointers.JetPackFuelPatch->Apply();
		g_Pointers.JetPackFuelPatch2->Apply();
	}
	else
	{
		g_Pointers.JetPackFuelPatch->Restore();
		g_Pointers.JetPackFuelPatch2->Restore();
	}
}


void Self::DrawMain()
{
	ImGui::Spacing();

	if (ImGui::Checkbox("God Mode", &InfiniteHealth) && g_Pointers.PlayerHealthPatch)
		InfiniteHealth ? g_Pointers.PlayerHealthPatch->Apply() : g_Pointers.PlayerHealthPatch->Restore();

	if (ImGui::Checkbox("Infinite Stamina", &InfiniteStamina) && g_Pointers.PlayerStaminaPatch)
		InfiniteStamina ? g_Pointers.PlayerStaminaPatch->Apply() : g_Pointers.PlayerStaminaPatch->Restore();

	if (ImGui::Checkbox("Infinite Life Support", &InfiniteLifeSupport) && g_Pointers.LifeSupportPatch)
		InfiniteLifeSupport ? g_Pointers.LifeSupportPatch->Apply() : g_Pointers.LifeSupportPatch->Restore();

	if (ImGui::Checkbox("Infinite Environmental Protection", &InfiniteEnvProtection) && g_Pointers.EnvProtectionPatch)
		InfiniteEnvProtection ? g_Pointers.EnvProtectionPatch->Apply() : g_Pointers.EnvProtectionPatch->Restore();

	if (ImGui::Checkbox("Infinite Jetpack", &InfiniteJetpack))
		ToggleInfiniteJetpack(&InfiniteJetpack);

	if (g_Pointers.GroundSpeed)
	{
		ImGui::SeparatorText("Ground Speed");

		if (ImGui::SliderFloat("##grndspd", &Self::GroundSpeed, 1.f, 20.f, "%.1f"))
		{
			*g_Pointers.GroundSpeed = Self::GroundSpeed;
		}

		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();

		if (ImGui::Button("Reset"))
			*g_Pointers.GroundSpeed = Self::DefaultGroundSpeed;
	}

	ImGui::Spacing();
	ImGui::SeparatorText(ICON_FA_MONEY_BILL);
	ImGui::Spacing();

	if (!g_Pointers.PlayerCurrency) {
		ImGui::TextColored(ImVec4(1.0f, 0.01f, 0.01f, 0.9f), "Currency data is not available! Try opening your inventory.");
		return;
	}

	for (const auto& item : m_CurrencyScalars) {
		uint32_t value = item.GetValue();
		ImGui::Spacing();

		if (ImGui::InputScalar(item.name, ImGuiDataType_U32, &value, &CurrencyStep, &CurrencyStepFast))
			item.SetValue(value);

		if (ImGui::IsItemHovered())
			ImGui::SetTooltip(FormatMoney(value).c_str());
	}
}

/*
void Self::DrawSpawner()
{
	ImGui::Text("Placeholder");
}
*/

void Self::Draw()
{
	DrawMain();

	/*
	if (ImGui::BeginTabBar("selfTabBar"))
	{
		if (ImGui::BeginTabItem("Main"))
		{
			DrawMain();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Item Spawner"))
		{
			DrawSpawner();
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	*/
}
