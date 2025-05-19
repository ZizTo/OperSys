#include "structs.h"
#include <windows.h>
#include <iostream>
#include <string>

using namespace std;

HANDLE hNamedPipe = INVALID_HANDLE_VALUE;
HANDLE hNeedToReadEvent = NULL;
HANDLE hICanReadEvent = NULL;
HANDLE hIDeadEvent = NULL;

int clientID = -1;

void CleanUpClient() {
    CloseHandle(hNeedToReadEvent);
    CloseHandle(hICanReadEvent);
    CloseHandle(hIDeadEvent);
    CloseHandle(hNamedPipe);
}

void DisplayEmployee(const Employee& emp) {
    cout<< "Client#" << emp.num << " - "
        << emp.name << " - "
        << emp.hours << endl;
}

Employee ModifyEmployee(Employee data) {
    cout << "Enter new name for employee #" << data.num << " (now: " << data.name << "): ";
    string newName;
    cin >> newName;
    strncpy_s(data.name, sizeof(data.name), newName.c_str(), _TRUNCATE);

    cout << "Enter new hours for employee #" << data.num << " (now: " << data.hours << "): ";
    cin >> data.hours;
    return data;
}

int main(int argc, char* argv[]) {
    clientID = atoi(argv[1]);
    cout << "Client #" << clientID << " started" << endl;

    hNeedToReadEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, "ReadEvent");

    string readNow = "YouCanReadNowClient" + to_string(clientID);
    hICanReadEvent = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, readNow.c_str());

    string deadNow = "DeadEventClient" + to_string(clientID);
    hIDeadEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, deadNow.c_str());

    hNamedPipe = CreateFile(
        "\\\\.\\pipe\\employeeDataPipe",
        GENERIC_READ | GENERIC_WRITE,
        0,                            
        NULL,                         
        OPEN_EXISTING,                
        0,                            
        NULL);

    if (hNamedPipe == INVALID_HANDLE_VALUE) {
        cerr << "err" << endl;
        CleanUpClient();
        return 1;
    }
    cout << "Client #" << clientID << ": Connected to named pipe." << endl;


    Message message;
    DWORD dwBytes;

    bool running = true;
    while (running) {
        cout << "r - Read, s - Send, q - Quit: " << endl;
        char choice;
        cin >> choice;

        message.clientId = clientID;

        switch (choice) {
        case 'q':
            running = false;
            SetEvent(hIDeadEvent);
            break;
        case 'r':
            message.type = READ_REQUEST;
            cout << "Enter ID to read: ";
            cin >> message.employeeId;

            if (!WriteFile(hNamedPipe, &message, sizeof(Message), &dwBytes, NULL)) {
                cerr << "err writing" << endl;
                running = false; break;
            }

            WaitForSingleObject(hICanReadEvent, INFINITE);
            ReadFile(hNamedPipe, &message, sizeof(Message), &dwBytes, NULL);

            if (message.type == SUCCESS_READ) {
                DisplayEmployee(message.employee);
            }
            else if (message.type == FAIL_READ) {
                cout << "No ID" << endl;
            }
            else if (message.type == BLOCK_RESPONSE) {
                cout << "Blocked" << endl;
            }

            break;

        case 's': 
            message.type = WRITE_REQUEST;
            cout << "employee ID to modify: ";
            cin >> message.employeeId;

            WriteFile(hNamedPipe, &message, sizeof(Message), &dwBytes, NULL);

            WaitForSingleObject(hICanReadEvent, INFINITE);
            ReadFile(hNamedPipe, &message, sizeof(Message), &dwBytes, NULL);

            if (message.type == SUCCESS_READ) {
                cout << "Current data:" << endl;
                DisplayEmployee(message.employee);
                message.employee = ModifyEmployee(message.employee);

                message.type = WRITE_REQUEST_READY;
                message.clientId = clientID;

                WriteFile(hNamedPipe, &message, sizeof(Message), &dwBytes, NULL);

                WaitForSingleObject(hICanReadEvent, INFINITE);

                ReadFile(hNamedPipe, &message, sizeof(Message), &dwBytes, NULL);

                if (message.type == SUCCESS) {
                    cout << "Recorded on server" << endl;
                    DisplayEmployee(message.employee);
                }
                else {
                    cout << "Fail" << endl;
                }
            }
            else if (message.type == FAIL_READ) {
                cout << "No ID" << endl;
            }
            else if (message.type == BLOCK_RESPONSE) {
                cout << "Blocked" << endl;
            }
            break;

        default:
            cout << "One more time" << endl;
            break;
        }
    }

    CleanUpClient();
    return 0;
}