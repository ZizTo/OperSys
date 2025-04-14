#include <windows.h>
#include <iostream>
#include <string>
#include <cstring>

struct SharedData {
    int read_idx;
    int write_idx;
    char message[20];
};

int main() {
    std::string filename;
    int num_records;
    std::cout << "Enter filename: ";
    std::cin >> filename;
    std::cout << "Enter number of records: ";
    std::cin >> num_records;

    // Создание файла
    HANDLE hFile = CreateFileA(filename.c_str(), GENERIC_READ | GENERIC_WRITE,
        0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to create file\n";
        return 1;
    }

    // Инициализация структуры данных
    SharedData data = {0, 0, {0}};
    DWORD bytesWritten;
    WriteFile(hFile, &data, sizeof(data), &bytesWritten, NULL);
    CloseHandle(hFile);

    // Создание семафоров
    HANDLE hFreeSlots = CreateSemaphoreA(NULL, num_records, num_records, "free_slots");
    HANDLE hUsedSlots = CreateSemaphoreA(NULL, 0, num_records, "used_slots");
    HANDLE hMutex = CreateMutexA(NULL, FALSE, "file_mutex");
    if (!hFreeSlots || !hUsedSlots || !hMutex) {
        std::cerr << "Failed to create synchronization objects\n";
        return 1;
    }

    // Запуск Sender
    STARTUPINFOA si = { sizeof(STARTUPINFOA) };
    PROCESS_INFORMATION pi;
    std::string cmd = "Sender.exe " + filename;
    if (!CreateProcessA(NULL, (LPSTR)cmd.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        std::cerr << "Failed to start Sender\n";
        return 1;
    }
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    // Ожидание готовности Sender
    HANDLE hSenderReady = OpenEventA(EVENT_ALL_ACCESS, FALSE, "SenderReady");
    if (hSenderReady == NULL || WaitForSingleObject(hSenderReady, INFINITE) != WAIT_OBJECT_0) {
        std::cerr << "Failed to wait for Sender\n";
        return 1;
    }

    // Основной цикл
    char command;
    while (true) {
        std::cout << "Enter command (r/q): ";
        std::cin >> command;
        if (command == 'q') break;

        if (command == 'r') {
            WaitForSingleObject(hUsedSlots, INFINITE); // Ждем сообщение
            WaitForSingleObject(hMutex, INFINITE);     // Блокируем доступ к файлу

            // Чтение данных
            hFile = CreateFileA(filename.c_str(), GENERIC_READ | GENERIC_WRITE,
                0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            ReadFile(hFile, &data, sizeof(data), &bytesWritten, NULL);
            std::cout << "Message: " << data.message << std::endl;

            // Обновление индексов (для общего случая)
            data.read_idx = (data.read_idx + 1) % num_records;
            SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
            WriteFile(hFile, &data, sizeof(data), &bytesWritten, NULL);
            CloseHandle(hFile);

            ReleaseMutex(hMutex);
            ReleaseSemaphore(hFreeSlots, 1, NULL); // Освобождаем слот
        }
    }

    // Очистка
    CloseHandle(hFreeSlots);
    CloseHandle(hUsedSlots);
    CloseHandle(hMutex);
    CloseHandle(hSenderReady);
    return 0;
}