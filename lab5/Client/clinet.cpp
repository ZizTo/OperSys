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
        "\\\\.\\pipe\\pipe", // ��� ������
        GENERIC_READ | GENERIC_WRITE, // ������ � ���������� � �����
        FILE_SHARE_READ | FILE_SHARE_WRITE, // ��������� ������ � ������ � �����
        (LPSECURITY_ATTRIBUTES)NULL, // ������ �� ���������
        OPEN_EXISTING, // ��������� ������������ �����
        FILE_ATTRIBUTE_NORMAL, // �������� �� ���������
        (HANDLE)NULL // �������������� ��������� ���
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
                hNamedPipe, // ���������� ������
                &message, // ����� ������ ��� ������ ������
                sizeof(Message), // ����� ������������ ������
                &dwBytesWrite, // ����� ���������� ������
                (LPOVERLAPPED)NULL // �������� ������ ����������
            );
            SetEvent(NeedToRead);
            WaitForSingleObject(ICanRead, INFINITE);
            ResetEvent(ICanRead);
            ReadFile(
                hNamedPipe, // ���������� ������
                &message, // ����� ������ ��� ����� ������
                sizeof(Message), // ����� �������� ������
                &dwBytesWrite, // ����� ����������� ������
                (LPOVERLAPPED)NULL // �������� ������ ����������
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
                hNamedPipe, // ���������� ������
                &message, // ����� ������ ��� ������ ������
                sizeof(Message), // ����� ������������ ������
                &dwBytesWrite, // ����� ���������� ������
                (LPOVERLAPPED)NULL // �������� ������ ����������
            );
            SetEvent(NeedToRead);
            WaitForSingleObject(ICanRead, INFINITE);
            ResetEvent(ICanRead);
            ReadFile(
                hNamedPipe, // ���������� ������
                &message, // ����� ������ ��� ����� ������
                sizeof(Message), // ����� �������� ������
                &dwBytesWrite, // ����� ����������� ������
                (LPOVERLAPPED)NULL // �������� ������ ����������
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
                    hNamedPipe, // ���������� ������
                    &message, // ����� ������ ��� ������ ������
                    sizeof(Message), // ����� ������������ ������
                    &dwBytesWrite, // ����� ���������� ������
                    (LPOVERLAPPED)NULL // �������� ������ ����������
                );
                SetEvent(NeedToRead);
                WaitForSingleObject(ICanRead, INFINITE);
                ResetEvent(ICanRead);
                ReadFile(
                    hNamedPipe, // ���������� ������
                    &message, // ����� ������ ��� ����� ������
                    sizeof(Message), // ����� �������� ������
                    &dwBytesWrite, // ����� ����������� ������
                    (LPOVERLAPPED)NULL // �������� ������ ����������
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