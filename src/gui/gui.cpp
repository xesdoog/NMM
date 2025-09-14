#include "gui.hpp"
#include "renderer.hpp"
#include "memory/pointers.hpp"
#include "game/features/self.hpp"
#include "game/features/ship.hpp"


void GUI::InitImpl() {
	auto size = Renderer::GetScreenSize();
    size.width = std::max((int)size.width, 1920);
    size.height = std::max((int)size.height, 1080);
    SetWindowSize(size);

	AddTab(ICON_FA_USER, [] { Self::Draw(); }, "Player");
	AddTab(ICON_FA_SPACE_SHUTTLE, [] { Ship::Draw(); }, "Spaceship");
	AddTab(ICON_FA_COG, [] { Ship::Draw(); }, "Settings");

    Renderer::AddRendererCallBack([&] { Draw(); }, -1);
}

bool GUI::AddTabImpl(const std::string& name, GuiCallBack&& callback, std::optional<std::string> hint)
{
    for (const auto& tab : m_Tabs)
    {
        if (tab.m_name == name)
            return false;
    }

    m_Tabs.push_back({ name, callback, hint });
    return true;
}

void GUI::DrawImpl()
{
    if (!m_IsOpen)
        return;

    ImGui::SetNextWindowSize(m_WindowSize, ImGuiCond_Once);
    ImGui::SetNextWindowPos(m_WindowPos, ImGuiCond_Once);
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    if (ImGui::Begin("##main", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground))
    {
        ImGui::SetNextWindowBgAlpha(0.8f);
        if (ImGui::BeginChild("##header", ImVec2(m_WindowSize.x - 10.0f, 70.0f), ImGuiChildFlags_Border))
        {
            float textWidth = ImGui::CalcTextSize("NMS Test Trainer").x;
            float avail = ImGui::GetWindowWidth();
            ImGui::Dummy(ImVec2((avail - textWidth) * 0.5f, 1));
            ImGui::SameLine();
            ImGui::Text("NMS Test Trainer");

            if (ImGui::Button("Unload"))
                g_Running = false;
        }
        ImGui::EndChild();

        if (ImGui::BeginChild("##sidebar", ImVec2(m_WindowSize.x * 0.16f, 0), ImGuiChildFlags_None))
        {
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 40);
            for (const auto& tab : m_Tabs)
            {
                bool isActive = (!m_ActiveTab.m_name.empty() && (tab.m_name == m_ActiveTab.m_name));
                float btn_width = isActive ? 100.0f : 50.0f;

                if (isActive)
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.6f, 1.0f, 1.0f));
                else
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

                if (ImGui::Button(tab.m_name.c_str(), ImVec2(btn_width, 0)))
                {
                    if (isActive)
                        m_ActiveTab = {};
                    else
                        SetActiveTab(tab.m_name, tab.m_callback, tab.m_hint);
                }
                if (ImGui::IsItemHovered() && tab.m_hint.has_value())
                    ImGui::SetTooltip(tab.m_hint.value().c_str());
                ImGui::PopStyleColor();
            }
            ImGui::PopStyleVar();
        }
        ImGui::EndChild();

        ImGui::SameLine();

        if (m_ActiveTab.m_callback)
        {
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 0.8f));
            if (ImGui::BeginChild("##tabfuncs", ImVec2(0, 0), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Border))
                m_ActiveTab.m_callback();

            ImGui::PopStyleColor();
            ImGui::EndChild();
        }
    }
    ImGui::PopStyleVar();
    ImGui::End();
}

void GUI::OverrideMouse()
{
    auto& io = ImGui::GetIO();
    auto isOpen = GUI::IsOpen();

    io.MouseDrawCursor = isOpen;
    isOpen ? io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse : io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
}

void GUI::CloseImpl()
{
    m_IsOpen = false;
    OverrideMouse();
}
