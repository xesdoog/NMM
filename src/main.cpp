#include <Windows.h>
#include "gui/renderer.hpp"
#include "hooks/hooking.hpp"
#include "memory/pointers.hpp"
#include "logging/logger.hpp"
#include "gui/test_ui.hpp"


static DWORD WINAPI MainThread(LPVOID)
{
    MSG msg = {};

	Logger::Log(INFO, "Initializing...");

    g_pointers.Init();
    Renderer::Init();
    Hooking::Init();
	GUI::Init();

    // test loop
    while (true)
    {
        if (GetAsyncKeyState(VK_INSERT) & 1)
			GUI::Toggle();

        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {

            if (msg.message == WM_QUIT)
                goto unload;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        Sleep(10);
    }

unload:
    Logger::Log(INFO, "Shutting down");
    Hooking::Destroy();
    Renderer::Destroy();
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, MainThread, nullptr, 0, nullptr);
    }
    return TRUE;
}
