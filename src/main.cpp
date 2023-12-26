#include "main.h"
#include "engines/engine_base.h"

using namespace std::chrono_literals;

BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID)
{
    AllocConsole();
    FILE *filePtr;
    freopen_s(&filePtr, "CONOUT$", "w", stdout);

    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH: {

        // REVIEW main loop
        std::this_thread::sleep_for(1s);

        MH_Initialize();

        Engine *eg = Engine::DeduceEngineType();
        eg->AttachHooks();
    }
    break;
    case DLL_PROCESS_DETACH: {
        MH_Uninitialize();
    }
    break;
    }
    return TRUE;
}