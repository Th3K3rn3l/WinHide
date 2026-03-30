#include <windows.h>
#include <iostream>
#include <string>

int main() {
    setlocale(LC_ALL, "Russian");
    std::string dllPath = "C:\\Users\\Jinx\\source\\repos\\WinHide\\x64\\Release\\HideDll.dll";

    std::cout << "Программа запущена. Наведи курсор на нужное окно и нажми F2..." << std::endl;

    while (true) {
        // Проверяем, нажата ли клавиша F2
        if (GetAsyncKeyState(VK_F2) & 0x8000) {
            POINT pt;
            GetCursorPos(&pt); // Получаем координаты курсора
            HWND hWnd = WindowFromPoint(pt); // Находим окно под курсором

            if (hWnd) {
                // Поднимаемся к родительскому окну (если кликнули в кнопку или поле внутри окна)
                HWND rootHWnd = GetAncestor(hWnd, GA_ROOT);

                DWORD pid;
                GetWindowThreadProcessId(rootHWnd, &pid);

                std::cout << "Выбрано окно. PID процесса: " << pid << ". Инжектим..." << std::endl;

                HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
                if (hProcess) {
                    LPVOID pRemotePath = VirtualAllocEx(hProcess, NULL, dllPath.size() + 1, MEM_COMMIT, PAGE_READWRITE);
                    WriteProcessMemory(hProcess, pRemotePath, dllPath.c_str(), dllPath.size() + 1, NULL);

                    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0,
                        (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA"),
                        pRemotePath, 0, NULL);

                    if (hThread) {
                        std::cout << "Успешно! Окно должно стать черным для записи." << std::endl;
                        CloseHandle(hThread);
                    }
                    CloseHandle(hProcess);
                }
                else {
                    std::cout << "Ошибка открытия процесса: " << GetLastError() << std::endl;
                }
            }
            Sleep(500); // Защита от дребезга клавиши
        }
        Sleep(10);
    }
    return 0;
}