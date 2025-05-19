#include "structs.h"
#include <vector>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;
HANDLE hNamedPipe;
HANDLE NeedToRead;
HANDLE ICanRead;
HANDLE IDead;

void CleanUp() {
    CloseHandle(NeedToRead);
    CloseHandle(ICanRead);
    CloseHandle(IDead);
    CloseHandle(hNamedPipe);
}

void DisplayEmployee(Employee emp) {
    cout << "Employee#" << emp.num << " - "
        << emp.name << " - "
        << emp.hours << endl;
}

Employee ModifyEmployee(Employee data) {
    cout << "Enter name: ";
    string messageStr;
    cin >> messageStr;
    strncpy_s(data.name, messageStr.c_str(), sizeof(data.name) - 1);

    cout << "Enter hours: ";
    cin >> data.hours;

    return data;
}

int main(int argc, char* argv[]) {
    int threadId = atoi(argv[1]);

    hNamedPipe = CreateFile(
        "\\\\.\\pipe\\pipe", // имя канала
        GENERIC_READ | GENERIC_WRITE, // читаем и записываем в канал
        FILE_SHARE_READ | FILE_SHARE_WRITE, // разрешаем чтение и запись в канал
        (LPSECURITY_ATTRIBUTES)NULL, // защита по умолчанию
        OPEN_EXISTING, // открываем существующий канал
        FILE_ATTRIBUTE_NORMAL, // атрибуты по умолчанию
        (HANDLE)NULL // дополнительных атрибутов нет
    );
    Message message;
    DWORD dwBytesWrite;
    NeedToRead = OpenEvent(EVENT_ALL_ACCESS, FALSE, "ReadEvent");
    ICanRead = OpenEvent(EVENT_ALL_ACCESS, FALSE, "Read"+threadId);
    IDead = OpenEvent(EVENT_ALL_ACCESS, FALSE, "Dead" + threadId);
    
    while (true) {
        cout << "q - exit, s - send, r - read: ";
        char choose;
        cin >> choose;
        switch (choose)
        {
        case 'q':
            SetEvent(IDead);
            CleanUp();
            return 1;
            break;
        case 'r':
            message.id = threadId;
            message.type = READ_REQUEST;
            cout << "wich id you want: ";
            cin >> message.employeeId;
            DWORD dwBytesWrite;
            WriteFile(
                hNamedPipe, // дескриптор канала
                &message, // адрес буфера для вывода данных
                sizeof(Message), // число записываемых байтов
                &dwBytesWrite, // число записанных байтов
                (LPOVERLAPPED)NULL // передача данных синхронная
            );
            SetEvent(NeedToRead);
            WaitForSingleObject(ICanRead, INFINITE);
            ResetEvent(ICanRead);
            ReadFile(
                hNamedPipe, // дескриптор канала
                &message, // адрес буфера для ввода данных
                sizeof(Message), // число читаемых байтов
                &dwBytesWrite, // число прочитанных байтов
                (LPOVERLAPPED)NULL // передача данных синхронная
            );
            if (message.type == BLOCK_RESPONSE) {
                cout << "Blocked" << endl;
            }
            else if (message.type == FAIL_READ) {
                cout << "No id" << endl;
            }
            else if (message.type == SUCCESS_READ){
                DisplayEmployee(message.employee);
            }
        case 's':
            message.id = threadId;
            message.type = WRITE_REQUEST;
            cout << "wich id you want: ";
            cin >> message.employeeId;
            WriteFile(
                hNamedPipe, // дескриптор канала
                &message, // адрес буфера для вывода данных
                sizeof(Message), // число записываемых байтов
                &dwBytesWrite, // число записанных байтов
                (LPOVERLAPPED)NULL // передача данных синхронная
            );
            SetEvent(NeedToRead);
            WaitForSingleObject(ICanRead, INFINITE);
            ResetEvent(ICanRead);
            ReadFile(
                hNamedPipe, // дескриптор канала
                &message, // адрес буфера для ввода данных
                sizeof(Message), // число читаемых байтов
                &dwBytesWrite, // число прочитанных байтов
                (LPOVERLAPPED)NULL // передача данных синхронная
            );
            if (message.type == BLOCK_RESPONSE) {
                cout << "Blocked" << endl;
            }
            else if (message.type == FAIL_READ) {
                cout << "No id" << endl;
            }
            else if (message.type == SUCCESS_READ) {
                DisplayEmployee(message.employee);
                message.employee = ModifyEmployee(message.employee);
                message.type = WRITE_REQUEST_READY;
                message.id = threadId;
                WriteFile(
                    hNamedPipe, // дескриптор канала
                    &message, // адрес буфера для вывода данных
                    sizeof(Message), // число записываемых байтов
                    &dwBytesWrite, // число записанных байтов
                    (LPOVERLAPPED)NULL // передача данных синхронная
                );
                SetEvent(NeedToRead);
                WaitForSingleObject(ICanRead, INFINITE);
                ResetEvent(ICanRead);
                ReadFile(
                    hNamedPipe, // дескриптор канала
                    &message, // адрес буфера для ввода данных
                    sizeof(Message), // число читаемых байтов
                    &dwBytesWrite, // число прочитанных байтов
                    (LPOVERLAPPED)NULL // передача данных синхронная
                );
                if (message.type = SUCCESS) {
                    DisplayEmployee(message.employee);
                }
            }
        default:
            cout << "one more time" << endl;
            break;
        }
        
    }

    
}