#include <MinHook.h>

import <chrono>;
import <iostream>;
import engine_base;
import pcengine;
import console;

BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID)
{
    AllocConsole();
    SetConsoleOutputCP(CP_UTF8);
    FILE *filePtr;
    freopen_s(&filePtr, "CONOUT$", "w", stdout);

    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH: {
        MH_Initialize();
        NeoServer::Init();
        Engine *engine = new PCEngine();
        engine->AttachHooks();
        engine->StartHookWatcher();
    }
    break;
    case DLL_PROCESS_DETACH: {
        MH_Uninitialize();
    }
    break;
    default:
        break;
    }
    return TRUE;
}
