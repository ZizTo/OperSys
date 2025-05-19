#include "structs.h"
#include <vector>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

string binFl;
vector<pair<int, bool>> accessedRecords; // pair of <recordId, isWriteAccess>

void DisplayEmployeeFile() {
    ifstream file(binFl, ios::binary);
    Employee data;
    cout << "\nEmployees:\n";
    while (file.read(reinterpret_cast<char*>(&data), sizeof(data))) {
        cout << "Employee#" << data.num << " - "
            << data.name << " - "
            << data.hours << endl;
    }
    cout << endl;
    file.close();
}

Employee FindEmployee(int id) {
    ifstream file(binFl, ios::binary);
    Employee data;
    while (file.read(reinterpret_cast<char*>(&data), sizeof(data))) {
        if (data.num == id) {
            file.close();
            return data;
        }
    }
    file.close();
    data.num = -1;
    return data;
}

void ChangeEmployee(int id, Employee emp) {
    fstream file(binFl, ios::binary | ios::in | ios::out);
    Employee data;
    while (!file.eof()) {
        streampos pos = file.tellg();
        file.read(reinterpret_cast<char*>(&data), sizeof(data));
        if (file.eof()) break;
        
        if (data.num == id) {
            file.seekp(pos);
            file.write(reinterpret_cast<char*>(&emp), sizeof(emp));
            break;
        }
    }
    file.close();
}

int main() {
    cout << "Enter file name: ";
    cin >> binFl;
    int count;
    cout << "Enter number of employees: ";
    cin >> count;

    ofstream onF(binFl, ios::binary);

    for (int i = 0; i < count; i++) {
        Employee data;
        data.num = i;

        cout << "Enter name: ";
        string messageStr;
        cin >> messageStr;
        strncpy_s(data.name, messageStr.c_str(), sizeof(data.name) - 1);

        cout << "Enter hours: ";
        cin >> data.hours;
        
        onF.write(reinterpret_cast<char*>(&data), sizeof(data));
    }
    onF.close();
    DisplayEmployeeFile();

    // Create a very simple pipe with easier name
    HANDLE hPipe = CreateNamedPipe(
        "\\\\.\\pipe\\EmployeePipe",      // simple pipe name
        PIPE_ACCESS_DUPLEX,               // read/write access
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,         // allow multiple instances
        0, 0, INFINITE, NULL);            // default buffer sizes and timeout
    
    if (hPipe == INVALID_HANDLE_VALUE) {
        cerr << "CreateNamedPipe failed. Error: " << GetLastError() << endl;
        return 1;
    }

    // Create the client process
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    
    // Simplify the client launch - use exact executable name
    if (!CreateProcess(NULL, (LPSTR)"Client.exe", NULL, NULL, FALSE,
                      CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
        cerr << "CreateProcess failed. Error: " << GetLastError() << endl;
        CloseHandle(hPipe);
        return 1;
    }

    cout << "Client process created. Waiting for connection..." << endl;
    
    // Wait for the client to connect
    if (!ConnectNamedPipe(hPipe, NULL) && GetLastError() != ERROR_PIPE_CONNECTED) {
        cerr << "ConnectNamedPipe failed. Error: " << GetLastError() << endl;
        CloseHandle(hPipe);
        return 1;
    }
    
    cout << "Client connected successfully!" << endl;
    
    // Simple message loop
    bool readOpen = true, modificOpen = true;
    
    while (true) {
        DWORD bytesRead;
        Message message;
        
        // Read client request
        BOOL result = ReadFile(hPipe, &message, sizeof(Message), &bytesRead, NULL);
        if (!result || bytesRead == 0) {
            // Client has disconnected or error occurred
            DWORD error = GetLastError();
            if (error == ERROR_BROKEN_PIPE) {
                cout << "Client disconnected." << endl;
            } else {
                cerr << "ReadFile failed. Error: " << error << endl;
            }
            break;
        }
        
        // Process the request
        Message response;
        response.id = message.id;
        
        switch (message.type) {
        case READ_REQUEST:
            if (readOpen) {
                response.employee = FindEmployee(message.employeeId);
                if (response.employee.num == -1) {
                    response.type = FAIL_READ;
                } else {
                    response.type = SUCCESS_READ;
                }
            } else {
                response.type = BLOCK_RESPONSE;
            }
            break;
            
        case WRITE_REQUEST:
            if (modificOpen) {
                response.employee = FindEmployee(message.employeeId);
                if (response.employee.num == -1) {
                    response.type = FAIL_READ;
                } else {
                    readOpen = false;
                    modificOpen = false;
                    response.type = SUCCESS_READ;
                }
            } else {
                response.type = BLOCK_RESPONSE;
            }
            break;
            
        case WRITE_REQUEST_READY:
            ChangeEmployee(message.employee.num, message.employee);
            response.type = SUCCESS;
            response.employee = message.employee;
            readOpen = true;
            modificOpen = true;
            break;
            
        default:
            cerr << "Unknown message type: " << message.type << endl;
            response.type = FAIL_READ;
            break;
        }
        
        // Send response to client
        DWORD bytesWritten;
        if (!WriteFile(hPipe, &response, sizeof(Message), &bytesWritten, NULL)) {
            cerr << "WriteFile failed. Error: " << GetLastError() << endl;
            break;
        }
    }
    
    DisplayEmployeeFile();
    cout << "Press Enter to close...";
    cin.ignore();
    cin.get();
    
    // Clean up
    CloseHandle(hPipe);
    return 0;
}