#include "self.hpp"
#include "memory/pointers.hpp"
#include "util/format_uint.hpp"


uint32_t CurrencyStep = 1e3;
uint32_t CurrencyStepFast = 1e5;

uint32_t Self::GetCurrency(uint32_t Currency::* field)
{
	return g_Pointers.PlayerCurrency ? g_Pointers.PlayerCurrency->*field : 0;
}

void Self::SetCurrency(uint32_t Currency::* field, uint32_t value)
{
	if (g_Pointers.PlayerCurrency)
		g_Pointers.PlayerCurrency->*field = value;
}

uint32_t Self::GetUnits()
{
	return GetCurrency(&Currency::Units);
}

uint32_t Self::GetNanites()
{
	return GetCurrency(&Currency::Nanites);
}

uint32_t Self::GetQuicksilver()
{
	return GetCurrency(&Currency::Quicksilver);
}

void Self::SetUnits(uint32_t amount)
{
	SetCurrency(&Currency::Units, amount);
}

void Self::SetNanites(uint32_t amount)
{
	SetCurrency(&Currency::Nanites, amount);
}

void Self::SetQuicksilver(uint32_t amount)
{
	SetCurrency(&Currency::Quicksilver, amount);
}

void Self::DrawMain()
{
	ImGui::Spacing();

	Gui::PatchToggle("God Mode", InfiniteHealth, g_Pointers.PlayerHealthPatch);

	Gui::PatchToggle("Infinite Stamina", InfiniteStamina, g_Pointers.PlayerStaminaPatch);

	Gui::PatchToggle("Infinite Life Support", InfiniteLifeSupport, g_Pointers.LifeSupportPatch);

	Gui::PatchToggle("Infinite Environmental Protection", InfiniteEnvProtection, g_Pointers.EnvProtectionPatch);

	Gui::PatchToggle("Infinite Exosuit Shields", InfiniteExosuitShields, g_Pointers.ExosuitShieldsPatch);

	Gui::PatchToggle("Infinite Jetpack",
		InfiniteJetpack,
		g_Pointers.JetPackFuelPatch,
		g_Pointers.JetPackFuelPatch2
	);

	Gui::PatchToggle("Super Jetpack",
		SuperJetpack,
		g_Pointers.JetpackPowerPatch,
		g_Pointers.JetpackPowerPatch1,
		g_Pointers.JetpackPowerPatch2
	);
	Gui::Tooltip("Greatly increases jetpack power.");

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
	ImGui::SeparatorText("Crafting");
	ImGui::Spacing();

	Gui::PatchToggle("Free Crafting", FreeCrafting, g_Pointers.FreeCraftingPatch);

	ImGui::Spacing();
	ImGui::SeparatorText("Currency");
	ImGui::Spacing();

	if (!g_Pointers.PlayerCurrency)
	{
		ImGui::TextColored(ImVec4(0.1f, 0.3f, 0.9f, 0.9f), "Currency data is not available! Try opening your inventory.");
		return;
	}

	for (const auto& item : m_CurrencyScalars)
	{
		uint32_t value = item.GetValue();
		ImGui::Spacing();

		if (ImGui::InputScalar(item.name, ImGuiDataType_U32, &value, &CurrencyStep, &CurrencyStepFast))
			item.SetValue(value);
		Gui::Tooltip(FormatUint(value).c_str());
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
