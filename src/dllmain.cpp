#include "includes.h"
#include "hooks.hpp"
#include "menu.hpp"

DWORD WINAPI my_thread(void* hModule) {
#ifdef GDL_INDEV
    AllocConsole();
    freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
    SetConsoleOutputCP(65001);
#endif // GDL_INDEV

    if (MH_Initialize() != MH_OK) {
        log << "Failed to initialize MinHook!";
        return 0;
    }

    hooks::main();
    menu::main();

    MH_EnableHook(MH_ALL_HOOKS);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0x1000, my_thread, hModule, 0, 0);
        break;
    }
    return TRUE;
}