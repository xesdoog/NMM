#include <Windows.h>
#include <common.hpp>
#include "gui/renderer.hpp"
#include "hooks/hooking.hpp"
#include "logger/log_helper.hpp"
#include "memory/pointers.hpp"
#include "gui/gui.hpp"


static DWORD WINAPI MainThread(LPVOID)
{
    g_ProjectPath = std::filesystem::path(std::getenv("appdata")) / "NMM";
    if (!std::filesystem::exists(g_ProjectPath))
        std::filesystem::create_directories(g_ProjectPath);

    LogHelper::Init("NoMansMenu", g_ProjectPath / "cout.log");
    LOG(INFO) << "Initializing...";

    if (!g_Pointers.Init())
        goto unload;

    if (!Renderer::Init())
        goto unload;

    if (!Hooking::Init())
        goto unload;

	GUI::Init();

    while (g_Running)
    {
        if (GetAsyncKeyState(VK_INSERT) & 1)
            GUI::Toggle();

        std::this_thread::sleep_for(10ms);
    }

unload:
    LOG(INFO) << "Shutting down";
    Renderer::Destroy();
    Hooking::Destroy();
    LogHelper::Destroy();
    CloseHandle(g_MainThread);
    FreeLibraryAndExitThread(g_DllInstance, EXIT_SUCCESS);
    return EXIT_SUCCESS;
}

BOOL WINAPI DllMain(HINSTANCE dllInstance, DWORD reason, void*)
{
    DisableThreadLibraryCalls(dllInstance);

    if (reason == DLL_PROCESS_ATTACH)
    {
        g_DllInstance = dllInstance;
        g_MainThread = CreateThread(nullptr, 0, MainThread, nullptr, 0, &g_MainThreadId);
    }

    return true;
}
