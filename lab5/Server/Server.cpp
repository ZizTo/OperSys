#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "structs.h"

using namespace std;

vector<Employee> employees;
vector<RecordLock> locks;

vector<HANDLE> pi;
vector<HANDLE> hi
string fileName;
HANDLE hPipe;
bool serverRunning = true;

int FindEmployeeById(int id) {
    for (size_t i = 0; i < employees.size(); i++) {
        if (employees[i].num == id) {
            return (int)i;
        }
    }
    return -1;
}

void Cleanup() {
    for (size_t i = 0; i < locks.size(); i++) {
        CloseHandle(locks[i].mutex);

    }
    CloseHandle()
}


RecordLock* FindOrCreateLock(int employeeId) {
    for (size_t i = 0; i < locks.size(); i++) {
        if (locks[i].employeeId == employeeId) {
            return &locks[i];
        }
    }

    // If not found, create new lock
    RecordLock lock;
    lock.employeeId = employeeId;
    lock.state = UNLOCKED;
    lock.readCount = 0;
    lock.mutex = CreateMutex(NULL, FALSE, NULL);
    locks.push_back(lock);

    return &locks[locks.size() - 1];
}

void CreateEmployeeFile() {
    ofstream file(fileName.c_str(), ios::binary);
    if (!file) {
        cerr << "Failed to create file!" << endl;
        exit(1);
    }

    int count;
    cout << "Enter number of employees: ";
    cin >> count;

    for (int i = 0; i < count; i++) {
        Employee emp;
        cout << "Employee #" << (i + 1) << " ID: ";
        cin >> emp.num;
        cout << "Employee #" << (i + 1) << " Name: ";
        cin >> emp.name;
        cout << "Employee #" << (i + 1) << " Hours: ";
        cin >> emp.hours;

        employees.push_back(emp);
        file.write(reinterpret_cast<char*>(&emp), sizeof(Employee));
    }

    file.close();
}

void DisplayEmployeeFile() {
    cout << "Employee File Content:" << endl;

    for (size_t i = 0; i < employees.size(); i++) {
        cout << employees[i].num << " - "
            << employees[i].name << " - "
            << employees[i].hours << endl;
    }
    cout << endl;
}

DWORD WINAPI ClientHandler(LPVOID lpParam) {
    HANDLE hPipe = (HANDLE)lpParam;
    Message message;
    DWORD bytesRead, bytesWritten;

    while (true) {
        // Read client message
        if (!ReadFile(hPipe, &message, sizeof(Message), &bytesRead, NULL) || bytesRead == 0) {
            break;
        }

        switch (message.type) {
        case READ_REQUEST: {
            // Handle read request
            int index = FindEmployeeById(message.employeeId);
            RecordLock* lock = FindOrCreateLock(message.employeeId);

            // Wait for mutex
            WaitForSingleObject(lock->mutex, INFINITE);

            if (index != -1 && (lock->state == UNLOCKED || lock->state == READ_LOCKED)) {
                // Acquire read lock
                lock->state = READ_LOCKED;
                lock->readCount++;

                // Release mutex
                ReleaseMutex(lock->mutex);

                // Send success response with employee data
                message.type = READ_RESPONSE;
                message.employee = employees[index];
                WriteFile(hPipe, &message, sizeof(Message), &bytesWritten, NULL);
            }
            else {
                // Release mutex
                ReleaseMutex(lock->mutex);

                // Send error response
                message.type = ERROR_RESPONSE;
                WriteFile(hPipe, &message, sizeof(Message), &bytesWritten, NULL);
            }
            break;
        }
        case WRITE_REQUEST: {
            // Handle write request
            int index = FindEmployeeById(message.employeeId);
            RecordLock* lock = FindOrCreateLock(message.employeeId);

            // Wait for mutex
            WaitForSingleObject(lock->mutex, INFINITE);

            if (index != -1 && lock->state == UNLOCKED) {
                // Acquire write lock
                lock->state = WRITE_LOCKED;

                // Release mutex
                ReleaseMutex(lock->mutex);

                // Send success response with employee data
                message.type = WRITE_RESPONSE;
                message.employee = employees[index];
                WriteFile(hPipe, &message, sizeof(Message), &bytesWritten, NULL);
            }
            else {
                // Release mutex
                ReleaseMutex(lock->mutex);

                // Send error response
                message.type = ERROR_RESPONSE;
                WriteFile(hPipe, &message, sizeof(Message), &bytesWritten, NULL);
            }
            break;
        }
        case WRITE_RESPONSE: {
            // Handle write response (update record)
            int index = FindEmployeeById(message.employeeId);

            if (index != -1) {
                // Update employee record
                employees[index] = message.employee;

                // Write updated records to file
                ofstream file(fileName.c_str(), ios::binary);
                for (size_t i = 0; i < employees.size(); i++) {
                    file.write(reinterpret_cast<char*>(&employees[i]), sizeof(Employee));
                }
                file.close();

                // Send success response
                message.type = SUCCESS_RESPONSE;
                WriteFile(hPipe, &message, sizeof(Message), &bytesWritten, NULL);
            }
            else {
                // Send error response
                message.type = ERROR_RESPONSE;
                WriteFile(hPipe, &message, sizeof(Message), &bytesWritten, NULL);
            }
            break;
        }
        case UNLOCK_REQUEST: {
            // Handle unlock request
            RecordLock* lock = FindOrCreateLock(message.employeeId);

            // Wait for mutex
            WaitForSingleObject(lock->mutex, INFINITE);

            if (lock->state == READ_LOCKED) {
                lock->readCount--;
                if (lock->readCount == 0) {
                    lock->state = UNLOCKED;
                }
            }
            else if (lock->state == WRITE_LOCKED) {
                lock->state = UNLOCKED;
            }

            // Release mutex
            ReleaseMutex(lock->mutex);

            // Send success response
            message.type = SUCCESS_RESPONSE;
            WriteFile(hPipe, &message, sizeof(Message), &bytesWritten, NULL);
            break;
        }
        default:
            break;
        }
    }

    DisconnectNamedPipe(hPipe);
    return 0;
}

void StartServer(int clientCount) {
    for (size_t i = 0; i < employees.size(); i++) {
        RecordLock lock;
        lock.employeeId = employees[i].num;
        lock.state = UNLOCKED;
        lock.readCount = 0;
        lock.mutex = CreateMutex(NULL, FALSE, NULL);
        locks.push_back(lock);
    }

    HANDLE hPipe = CreateNamedPipe(
        "\\\\.\\pipe\\EmployeeServer",
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        sizeof(Message),
        sizeof(Message),
        0,
        NULL);

    if (hPipe == INVALID_HANDLE_VALUE) {
        cerr << "CreateNamedPipe failed: " << GetLastError() << endl;
        return;
    }

    cout << "Server started. Launching client processes...\n";

    for (int i = 0; i < clientCount; i++) {
        STARTUPINFO si;
        PROCESS_INFORMATION pi;

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        if (!CreateProcess(
            "Client98.exe",  // Client executable name
            NULL,          // Command line
            NULL,          // Process handle not inheritable
            NULL,          // Thread handle not inheritable
            FALSE,         // Set handle inheritance to FALSE
            0,             // No creation flags
            NULL,          // Use parent's environment block
            NULL,          // Use parent's starting directory
            &si,           // Pointer to STARTUPINFO structure
            &pi)           // Pointer to PROCESS_INFORMATION structure
            ) {
            cerr << "CreateProcess failed: " << GetLastError() << endl;
            continue;
        }

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    while (serverRunning) {
        cout << "Waiting for client connection..." << endl;

        if (ConnectNamedPipe(hPipe, NULL) || GetLastError() == ERROR_PIPE_CONNECTED) {
            cout << "Client connected." << endl;

            // Create thread to handle client
            HANDLE hThread = CreateThread(
                NULL,                   // Default security attributes
                0,                      // Default stack size
                ClientHandler,          // Thread function
                (LPVOID)hPipe,          // Parameter to thread function
                0,                      // Default creation flags
                NULL);                  // Thread identifier

            if (hThread == NULL) {
                cerr << "CreateThread failed: " << GetLastError() << endl;
                DisconnectNamedPipe(hPipe);
                continue;
            }

            CloseHandle(hThread);

            // Create a new pipe instance for the next client
            hPipe = CreateNamedPipe(
                "\\\\.\\pipe\\EmployeeServer",
                PIPE_ACCESS_DUPLEX,
                PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
                PIPE_UNLIMITED_INSTANCES,
                sizeof(Message),
                sizeof(Message),
                0,
                NULL);

            if (hPipe == INVALID_HANDLE_VALUE) {
                cerr << "CreateNamedPipe failed: " << GetLastError() << endl;
                break;
            }
        }
    }

    CloseHandle(hPipe);
}

int main() {
    int clientCount;

    cout << "Enter file name: ";
    cin >> fileName;

    CreateEmployeeFile();
    DisplayEmployeeFile();

    cout << "Enter number of client processes to launch: ";
    cin >> clientCount;

    StartServer(clientCount);

    // After all clients finish
    DisplayEmployeeFile();

    cout << "Press Enter to exit...";
    cin.ignore();
    cin.get();

    CleanupLocks();
    return 0;
}