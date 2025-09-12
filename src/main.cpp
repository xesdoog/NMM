#include <Windows.h>
#include <common.hpp>
#include "gui/renderer.hpp"
#include "hooks/hooking.hpp"
#include "memory/pointers.hpp"
#include "logging/logger.hpp"
#include "gui/gui.hpp"


static DWORD WINAPI MainThread(LPVOID)
{
    Logger::Log(INFO, "Initializing...");
    
    if (!g_pointers.Init())
        goto unload;

    if (!Renderer::Init())
        goto unload;

    if (!Hooking::Init())
        goto unload;

	GUI::Init();

    // test loop
    while (g_Running)
    {
        if (GetAsyncKeyState(VK_INSERT) & 1)
            GUI::Toggle();
        Sleep(10);
    }

unload:
    Logger::Log(INFO, "Shutting down");
    Renderer::Destroy();
    Hooking::Destroy();
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
