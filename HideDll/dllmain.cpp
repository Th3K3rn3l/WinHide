#include <windows.h>
#include "pch.h"

void HideAllWindows() {
    DWORD currentPid = GetCurrentProcessId();
    HWND hWnd = GetTopWindow(NULL); // Начинаем с самого верхнего окна в системе

    while (hWnd) {
        DWORD windowPid;
        GetWindowThreadProcessId(hWnd, &windowPid);

        // Если это окно нашего процесса
        if (windowPid == currentPid && IsWindowVisible(hWnd)) {
            // сделать прозрачным для записи
            SetWindowDisplayAffinity(hWnd, WDA_EXCLUDEFROMCAPTURE);
            LONG_PTR exStyle = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
            // Скрыть из alt + tab и убрать из панели задач
            exStyle |= WS_EX_TOOLWINDOW;
            exStyle &= ~WS_EX_APPWINDOW;

            SetWindowLongPtr(hWnd, GWL_EXSTYLE, exStyle);
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