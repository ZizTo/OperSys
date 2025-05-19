#include "structs.h"
#include <vector>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

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

int main() {
    // Connect to the server's pipe with simplified name
    HANDLE hPipe = CreateFile(
        "\\\\.\\pipe\\EmployeePipe",  // simplified pipe name
        GENERIC_READ | GENERIC_WRITE, // read/write access
        0,                            // no sharing
        NULL,                         // default security
        OPEN_EXISTING,                // open existing pipe
        0,                            // default attributes
        NULL                          // no template file
    );
    
    if (hPipe == INVALID_HANDLE_VALUE) {
        cout << "Failed to connect to pipe. Error: " << GetLastError() << endl;
        cout << "Make sure the server is running first!" << endl;
        cout << "Press Enter to exit...";
        cin.get();
        return 1;
    }
    
    cout << "Connected to server!" << endl;
    
    // Main client loop
    while (true) {
        cout << "q - exit, r - read, s - send: ";
        char choose;
        cin >> choose;
        
        if (choose == 'q') {
            // Exit the program
            break;
        }
        
        DWORD bytesWritten, bytesRead;
        Message message;
        message.id = 0;  // Just use 0 as the client ID in simplified version
        
        if (choose == 'r') {
            // Read an employee record
            message.type = READ_REQUEST;
            cout << "Enter employee ID you want to read: ";
            cin >> message.employeeId;
            
            // Send request to server
            if (!WriteFile(hPipe, &message, sizeof(Message), &bytesWritten, NULL)) {
                cerr << "WriteFile failed. Error: " << GetLastError() << endl;
                break;
            }
            
            // Read response from server
            if (!ReadFile(hPipe, &message, sizeof(Message), &bytesRead, NULL)) {
                cerr << "ReadFile failed. Error: " << GetLastError() << endl;
                break;
            }
            
            // Process response
            if (message.type == BLOCK_RESPONSE) {
                cout << "Access blocked - record is being modified." << endl;
            }
            else if (message.type == FAIL_READ) {
                cout << "Employee ID not found." << endl;
            }
            else if (message.type == SUCCESS_READ) {
                cout << "Record found:" << endl;
                DisplayEmployee(message.employee);
            }
        }
        else if (choose == 's') {
            // Modify an employee record
            message.type = WRITE_REQUEST;
            cout << "Enter employee ID you want to modify: ";
            cin >> message.employeeId;
            
            // Send request to server
            if (!WriteFile(hPipe, &message, sizeof(Message), &bytesWritten, NULL)) {
                cerr << "WriteFile failed. Error: " << GetLastError() << endl;
                break;
            }
            
            // Read response from server
            if (!ReadFile(hPipe, &message, sizeof(Message), &bytesRead, NULL)) {
                cerr << "ReadFile failed. Error: " << GetLastError() << endl;
                break;
            }
            
            // Process response
            if (message.type == BLOCK_RESPONSE) {
                cout << "Access blocked - record is already being accessed." << endl;
            }
            else if (message.type == FAIL_READ) {
                cout << "Employee ID not found." << endl;
            }
            else if (message.type == SUCCESS_READ) {
                cout << "Current record:" << endl;
                DisplayEmployee(message.employee);
                
                cout << "Enter new information:" << endl;
                message.employee = ModifyEmployee(message.employee);
                message.type = WRITE_REQUEST_READY;
                
                // Send modified record to server
                if (!WriteFile(hPipe, &message, sizeof(Message), &bytesWritten, NULL)) {
                    cerr << "WriteFile failed. Error: " << GetLastError() << endl;
                    break;
                }
                
                // Read confirmation from server
                if (!ReadFile(hPipe, &message, sizeof(Message), &bytesRead, NULL)) {
                    cerr << "ReadFile failed. Error: " << GetLastError() << endl;
                    break;
                }
                
                if (message.type == SUCCESS) {
                    cout << "Record successfully updated:" << endl;
                    DisplayEmployee(message.employee);
                }
                else {
                    cout << "Failed to update record." << endl;
                }
            }
        }
        else {
            cout << "Invalid option. Please try again." << endl;
        }
    }
    
    // Clean up
    CloseHandle(hPipe);
    cout << "Disconnected from server. Press Enter to exit...";
    cin.ignore();
    cin.get();
    
    return 0;
}