#include "gui.hpp"
#include "renderer.hpp"
#include "memory/pointers.hpp"
#include "game/features/self.hpp"
#include "game/features/multitool.hpp"
#include "game/features/ship.hpp"


void GUI::DrawSettingsImpl()
{
    if (ImGui::Checkbox("Unlock Menu Positioning", &m_Movable))
        m_Movable ? m_MainWindowFlags &= ~ImGuiWindowFlags_NoMove : m_MainWindowFlags |= ImGuiWindowFlags_NoMove;

    Gui::Tooltip("Allows you to drag and move the menu around.");

    ImGui::SameLine();
    ImGui::Spacing();
    ImGui::SameLine();

    if (ImGui::Button("Snap Back"))
    {
        m_Movable = false;
        m_ShouldSnap = true;
		m_MainWindowFlags |= ImGuiWindowFlags_NoMove;
    }

    Gui::Tooltip("Snaps the menu back to the top left corner and locks its position.");

    ImGui::Spacing();
    ImGui::SeparatorText("Theme");
    ImGui::ShowStyleSelector("##styleselector");
}

void GUI::InitImpl()
{
    auto size = Renderer::GetScreenSize();
    size.width = std::max((int)size.width, 1920);
    size.height = std::max((int)size.height, 1080);
    SetWindowSize(size);

    AddTab(ICON_FA_USER, [] { Self::Draw(); }, "Player");
    AddTab(ICON_FA_CROSSHAIRS, [] { Multitool::Draw(); }, "Multitool");
    AddTab(ICON_FA_SPACE_SHUTTLE, [] { Ship::Draw(); }, "Starship");
    AddTab(ICON_FA_COG, [] { DrawSettings(); }, "Settings");

    Renderer::AddRendererCallBack([&] { Draw(); }, -1);
    Toggle();
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

    ImGui::SetNextWindowPos(m_WindowPos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(m_WindowSize, ImGuiCond_Always);

    if (m_ShouldSnap)
    {
        ImGui::SetNextWindowPos(m_WindowPos, ImGuiCond_Always);
        m_ShouldSnap = false;
    }

    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    if (ImGui::Begin("##main", nullptr, m_MainWindowFlags))
    {
        ImGui::SetNextWindowBgAlpha(0.8f);
        if (ImGui::BeginChild("##header", ImVec2(m_WindowSize.x - 10.0f, 70.0f), ImGuiChildFlags_Border))
        {
            float textWidth = ImGui::CalcTextSize("No Man's Menu").x;
            float avail = ImGui::GetWindowWidth();
            ImGui::Dummy(ImVec2((avail - textWidth) * 0.5f, 1));
            ImGui::SameLine();
            ImGui::Text("No Man's Menu");

            if (ImGui::Button("Unload"))
                g_Running = false;

            Gui::Tooltip("WARNING: This currently crashes the game!");
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
                if (tab.m_hint.has_value())
                    Gui::Tooltip(tab.m_hint.value().c_str());

                ImGui::PopStyleColor();
            }
            ImGui::PopStyleVar();
        }
        ImGui::EndChild();

        if (m_ActiveTab.m_callback)
        {
            ImGui::SameLine();
            float avail_x = ImGui::GetContentRegionAvail().x;
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 0.8f));
            ImGui::SetNextWindowSizeConstraints(ImVec2(avail_x, 60.f), ImVec2(avail_x, m_WindowSize.y * 0.6));
            // an if statement here is redundant since ImGuiChildFlags_AlwaysAutoResize makes BeginChild always return true.
            ImGui::BeginChild("##tabfuncs", ImVec2(0, 0), ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize);
            m_ActiveTab.m_callback();
            ImGui::EndChild();
            ImGui::PopStyleColor();
        }
    }
    ImGui::End();
    ImGui::PopStyleVar();
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
    Renderer::SetSafeToRender(false);
    m_IsOpen = false;

    auto& io = ImGui::GetIO();
    io.MouseDrawCursor = false;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
}
