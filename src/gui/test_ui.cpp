#include "gui/renderer.hpp"
#include "gui/test_ui.hpp"
#include "memory/pointers.hpp"


void GUI::InitImpl() {
    static std::string txt_buff{};

    Renderer::AddRendererCallBack(
        [&] {
            if (!GUI::IsOpen())
                return;

            ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_Once);
            ImGui::Begin("Test Window", nullptr, ImGuiWindowFlags_NoTitleBar);
            ImGui::Text("Hello from No Man's Sky!");
			ImGui::Separator();
			ImGui::Spacing();

            if (ImGui::Button("Get Currency Data")) {
                auto curr = g_pointers.PlayerCurrency;

                if (curr && !IsBadReadPtr(curr, sizeof(Currency))) {
					txt_buff = std::format(
                        "Currency Data:\nUnits: {}\nNanites: {}\nQuicksilver: {}",
                        curr->Units,
                        curr->Nanites,
                        curr->Quicksilver
                    );
                } else {
                    txt_buff = "Currency pointer was null!";
                }
            }

			ImGui::TextWrapped("%s", txt_buff.c_str());
            ImGui::End();
        },
    -1);
}

void GUI::OverrideMouse()
{
    auto& io = ImGui::GetIO();
    auto isOpen = GUI::IsOpen();

    io.MouseDrawCursor = isOpen;
    isOpen ? io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse : io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
}
