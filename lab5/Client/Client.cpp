#include <windows.h>
#include <iostream>
#include <string>
#include "structs.h"

using namespace std;

void DisplayEmployee(Employee emp) {
    cout << "ID: " << emp.num << endl;
    cout << "Name: " << emp.name << endl;
    cout << "Hours: " << emp.hours << endl;
}

void ModifyRecord(HANDLE hPipe) {
    Message message;
    DWORD bytesWritten, bytesRead;

    cout << "Enter employee ID: ";
    cin >> message.employeeId;
 
    message.type = WRITE_REQUEST;
    if (!WriteFile(hPipe, &message, sizeof(Message), &bytesWritten, NULL)) {
        cerr << "WriteFile failed: " << GetLastError() << endl;
        return;
    }

    if (!ReadFile(hPipe, &message, sizeof(Message), &bytesRead, NULL)) {
        cerr << "ReadFile failed: " << GetLastError() << endl;
        return;
    }

    if (message.type == WRITE_RESPONSE) {
        cout << "Current record:" << endl;
        DisplayEmployee(message.employee);

        cout << "Enter new values:" << endl;
        cout << "Name: ";
        cin >> message.employee.name;
        cout << "Hours: ";
        cin >> message.employee.hours;

        message.type = WRITE_RESPONSE;
        if (!WriteFile(hPipe, &message, sizeof(Message), &bytesWritten, NULL)) {
            cerr << "WriteFile failed: " << GetLastError() << endl;
            return;
        }

        if (!ReadFile(hPipe, &message, sizeof(Message), &bytesRead, NULL)) {
            cerr << "ReadFile failed: " << GetLastError() << endl;
            return;
        }

        if (message.type == SUCCESS_RESPONSE) {
            cout << "Record updated successfully." << endl;
        }
        else {
            cout << "Failed to update record." << endl;
        }

        cout << "Press Enter to release lock...";
        cin.ignore();
        cin.get();

        message.type = UNLOCK_REQUEST;
        if (!WriteFile(hPipe, &message, sizeof(Message), &bytesWritten, NULL)) {
            cerr << "WriteFile failed: " << GetLastError() << endl;
            return;
        }

        if (!ReadFile(hPipe, &message, sizeof(Message), &bytesRead, NULL)) {
            cerr << "ReadFile failed: " << GetLastError() << endl;
            return;
        }
    }
    else {
        cout << "Failed to acquire lock for writing. Record may be locked by another client." << endl;
    }
}

void ReadRecord(HANDLE hPipe) {
    Message message;
    DWORD bytesWritten, bytesRead;

    cout << "Enter employee ID: ";
    cin >> message.employeeId;

    message.type = READ_REQUEST;
    if (!WriteFile(hPipe, &message, sizeof(Message), &bytesWritten, NULL)) {
        cerr << "WriteFile failed: " << GetLastError() << endl;
        return;
    }

    if (!ReadFile(hPipe, &message, sizeof(Message), &bytesRead, NULL)) {
        cerr << "ReadFile failed: " << GetLastError() << endl;
        return;
    }

    if (message.type == READ_RESPONSE) {
        cout << "Record found:" << endl;
        DisplayEmployee(message.employee);

        cout << "Press Enter to release lock...";
        cin.ignore();
        cin.get();

        message.type = UNLOCK_REQUEST;
        if (!WriteFile(hPipe, &message, sizeof(Message), &bytesWritten, NULL)) {
            cerr << "WriteFile failed: " << GetLastError() << endl;
            return;
        }

        if (!ReadFile(hPipe, &message, sizeof(Message), &bytesRead, NULL)) {
            cerr << "ReadFile failed: " << GetLastError() << endl;
            return;
        }
    }
    else {
        cout << "Failed to read record. Record may be locked for writing by another client." << endl;
    }
}

int main() {
    HANDLE hPipe = CreateFile(
        "\\\\.\\pipe\\EmployeeServer",
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (hPipe == INVALID_HANDLE_VALUE) {
        cerr << "Failed to connect to server. Error: " << GetLastError() << endl;
        return 1;
    }

    cout << "Connected to server." << endl;

    bool running = true;
    while (running) {
        cout << "Select operation:" << endl;
        cout << "1. Modify record" << endl;;
        cout << "2. Read record" << endl;
        cout << "3. Exit" << endl;
        cout << "Choice: ";

        int choice;
        cin >> choice;

        switch (choice) {
        case 1:
            ModifyRecord(hPipe);
            break;
        case 2:
            ReadRecord(hPipe);
            break;
        case 3:
            running = false;
            break;
        default:
            cout << "Cringe. One more time" << endl;
            break;
        }
    }

    CloseHandle(hPipe);
    return 0;
}