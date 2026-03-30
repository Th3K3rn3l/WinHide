#include <windows.h>
#include "pch.h"

// Функция для возврата окна в нормальное состояние
void RestoreAllWindows() {
    DWORD currentPid = GetCurrentProcessId();
    HWND hWnd = GetTopWindow(NULL);

    while (hWnd) {
        DWORD windowPid;
        GetWindowThreadProcessId(hWnd, &windowPid);

        if (windowPid == currentPid) {
            // 1. Возвращаем видимость для записи (убираем защиту)
            SetWindowDisplayAffinity(hWnd, WDA_NONE);

            // 2. Возвращаем в Alt+Tab и Панель задач
            LONG_PTR exStyle = GetWindowLongPtr(hWnd, GWL_EXSTYLE);

            // Убираем WS_EX_TOOLWINDOW (маска через NOT и AND)
            exStyle &= ~WS_EX_TOOLWINDOW;
            // Возвращаем WS_EX_APPWINDOW (обычное окно приложения)
            exStyle |= WS_EX_APPWINDOW;

            SetWindowLongPtr(hWnd, GWL_EXSTYLE, exStyle);
        }
        hWnd = GetNextWindow(hWnd, GW_HWNDNEXT);
    }
}

// Твоя функция скрытия
void HideAllWindows() {
    DWORD currentPid = GetCurrentProcessId();
    HWND hWnd = GetTopWindow(NULL);

    while (hWnd) {
        DWORD windowPid;
        GetWindowThreadProcessId(hWnd, &windowPid);

        if (windowPid == currentPid && IsWindowVisible(hWnd)) {
            SetWindowDisplayAffinity(hWnd, WDA_EXCLUDEFROMCAPTURE);

            LONG_PTR exStyle = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
            exStyle |= WS_EX_TOOLWINDOW;
            exStyle &= ~WS_EX_APPWINDOW;

            SetWindowLongPtr(hWnd, GWL_EXSTYLE, exStyle);
        }
        hWnd = GetNextWindow(hWnd, GW_HWNDNEXT);
    }
}

DWORD WINAPI MainThread(LPVOID lpParam) {
    HMODULE hModule = (HMODULE)lpParam;

    // Сразу при инжекте скрываем окна
    HideAllWindows();

    // Цикл ожидания клавиши F4 для реверса
    while (true) {
        // Проверяем нажатие F4
        if (GetAsyncKeyState(VK_F4) & 0x8000) {
            RestoreAllWindows();
            break; // Выходим из цикла
        }
        Sleep(100); // Чтобы не грузить процессор на 100%
    }

    // Правильный выход: DLL сама выгружается из памяти чужого процесса
    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        // Передаем hModule в поток, чтобы он мог вызвать FreeLibraryAndExitThread
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MainThread, hModule, 0, NULL);
    }
    return TRUE;
}