#include "includes.hpp"
#include "hooks.hpp"
#include "menu.hpp"

DWORD WINAPI my_thread(void* hModule) {
#ifdef GDL_INDEV
    AllocConsole();
    freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
    SetConsoleOutputCP(65001);
#endif // GDL_INDEV

    if (MH_STATUS result = MH_Initialize(); result != MH_OK && result != MH_ERROR_ALREADY_INITIALIZED) {
        logD("Failed to initialize MinHook!");
        return 0;
    }

    hooks::main();

    MH_EnableHook(MH_ALL_HOOKS);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        // CreateThread(0, 0x1000, my_thread, hModule, 0, 0);
        hooks::initPatches();

#ifdef GDL_INDEV
        AllocConsole();
        freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
        SetConsoleOutputCP(65001);
#endif // GDL_INDEV

        if (MH_STATUS result = MH_Initialize(); result != MH_OK && result != MH_ERROR_ALREADY_INITIALIZED) {
            logD("Failed to initialize MinHook!");
            return 0;
        }

        hooks::main();

        MH_EnableHook(MH_ALL_HOOKS);

        break;
    }
    return TRUE;
}