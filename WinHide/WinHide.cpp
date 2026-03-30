#include <windows.h>
#include <iostream>
#include <string>

HWND g_targetHWnd = NULL; // Глобальная переменная для хранения окна-призрака

int main() {
    setlocale(LC_ALL, "Russian");
    std::string dllPath = "C:\\Users\\Jinx\\source\\repos\\WinHide\\x64\\Release\\HideDll.dll";

    std::cout << "[F2] - Сделать окно призраком | [F3] - Вернуть окно на передний план\n[F4] - Вернуть окно в исходное состояние" << std::endl;
    while (true) {
        // СКРЫТИЕ (F2)
        if (GetAsyncKeyState(VK_F2) & 0x8000) {
            POINT pt;
            GetCursorPos(&pt);
            HWND hWnd = WindowFromPoint(pt);

            if (hWnd) {
                g_targetHWnd = GetAncestor(hWnd, GA_ROOT); // Запоминаем окно
                DWORD pid;
                GetWindowThreadProcessId(g_targetHWnd, &pid);

                HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
                if (hProcess) {
                    LPVOID pRemotePath = VirtualAllocEx(hProcess, NULL, dllPath.size() + 1, MEM_COMMIT, PAGE_READWRITE);
                    WriteProcessMemory(hProcess, pRemotePath, dllPath.c_str(), dllPath.size() + 1, NULL);

                    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0,
                        (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA"),
                        pRemotePath, 0, NULL);

                    if (hThread) {
                        std::cout << "Окно успешно скрыто отовсюду!" << std::endl;
                        CloseHandle(hThread);
                    }
                    CloseHandle(hProcess);
                }
            }
            Sleep(500);
        }

        // АКТИВАЦИЯ (F3)
        if (GetAsyncKeyState(VK_F3) & 0x8000 && g_targetHWnd != NULL) {
            // Магия: вытаскиваем окно из небытия прямо тебе под руки
            ShowWindow(g_targetHWnd, SW_RESTORE);
            SetForegroundWindow(g_targetHWnd);
            std::cout << "Окно активировано." << std::endl;
            Sleep(500);
        }

        Sleep(10);
    }
    return 0;
}