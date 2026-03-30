#include <windows.h>
#include "pch.h"

void HideAllWindows() {
    DWORD currentPid = GetCurrentProcessId();
    HWND hWnd = GetTopWindow(NULL); // Начинаем с самого верхнего окна в системе

    while (hWnd) {
        DWORD windowPid;
        GetWindowThreadProcessId(hWnd, &windowPid);

        // Если это окно нашего процесса — скрываем
        if (windowPid == currentPid && IsWindowVisible(hWnd)) {
            SetWindowDisplayAffinity(hWnd, WDA_EXCLUDEFROMCAPTURE);
        }
        hWnd = GetNextWindow(hWnd, GW_HWNDNEXT); // Переходим к следующему
    }
}

DWORD WINAPI MainThread(LPVOID lpParam) {
    HideAllWindows();
    // Можно добавить FreeLibraryAndExitThread, чтобы DLL выгрузилась после работы
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MainThread, NULL, 0, NULL);
    }
    return TRUE;
}