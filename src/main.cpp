// #include "engines/engine_base.h"
#include <Windows.h>
#include <MinHook.h>
#include <chrono>

using namespace std::chrono_literals;

BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID)
{
    AllocConsole();
    SetConsoleOutputCP(CP_UTF8);
    FILE *filePtr;
    freopen_s(&filePtr, "CONOUT$", "w", stdout);

    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH: {

        // REVIEW main loop

        MH_Initialize();

        // Engine *eg = Engine::DeduceEngineType();
        // eg->AttachHooks();
    }
    break;
    case DLL_PROCESS_DETACH: {
        MH_Uninitialize();
    }
    break;
    }
    return TRUE;
}