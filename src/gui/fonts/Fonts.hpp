#pragma once
#include <imgui.h>
#include "FontAwesome.hpp"
#include "JetBrainsMono.hpp"

#define ICON_MIN_FA 0xF004
#define ICON_MAX_FA 0xF4FB


namespace Fonts {
    static inline void Load()
    {
        ImGuiIO& io = ImGui::GetIO();
        ImFontConfig mainCfg;
        strcpy(mainCfg.Name, "JetBrains Mono");
        io.Fonts->AddFontFromMemoryCompressedTTF(
            JetBrainsMono_compressed_data,
            JetBrainsMono_compressed_size,
            18.0f,
            &mainCfg
        );

        ImFontConfig iconCfg;
        iconCfg.MergeMode = true;
        iconCfg.PixelSnapH = true;
        strcpy(iconCfg.Name, "Font Awesome");
        static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
        io.Fonts->AddFontFromMemoryCompressedTTF(
            FontAwesome_compressed_data,
            FontAwesome_compressed_size,
            16.0f,
            &iconCfg,
            icon_ranges
        );
    }
}
