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
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <client_id>" << endl;
        return 1;
    }
    
    int threadId = atoi(argv[1]);
    
    // Create a unique pipe name based on the client ID
    string pipeName = "\\\\.\\pipe\\pipe" + to_string(threadId);
    
    hNamedPipe = CreateFile(
        pipeName.c_str(),  // unique pipe name for this client
        GENERIC_READ | GENERIC_WRITE, // read and write access
        FILE_SHARE_READ | FILE_SHARE_WRITE, // share read and write
        (LPSECURITY_ATTRIBUTES)NULL, // default security
        OPEN_EXISTING, // open existing pipe
        FILE_ATTRIBUTE_NORMAL, // default attributes
        (HANDLE)NULL // no template file
    );
    
    if (hNamedPipe == INVALID_HANDLE_VALUE) {
        cerr << "Failed to connect to pipe. Error: " << GetLastError() << endl;
        return 1;
    }
    
    Message message;
    DWORD dwBytesWrite;
    DWORD dwBytesRead;
    
    // Create unique event names based on client ID
    string readEventName = "ReadEvent" + to_string(threadId);
    string responseEventName = "Read" + to_string(threadId);
    string deadEventName = "Dead" + to_string(threadId);
    
    NeedToRead = OpenEvent(EVENT_ALL_ACCESS, FALSE, readEventName.c_str());
    if (!NeedToRead) {
        cerr << "Failed to open " << readEventName << ". Error: " << GetLastError() << endl;
        CloseHandle(hNamedPipe);
        return 1;
    }
    
    ICanRead = OpenEvent(EVENT_ALL_ACCESS, FALSE, responseEventName.c_str());
    if (!ICanRead) {
        cerr << "Failed to open " << responseEventName << ". Error: " << GetLastError() << endl;
        CloseHandle(NeedToRead);
        CloseHandle(hNamedPipe);
        return 1;
    }
    
    IDead = OpenEvent(EVENT_ALL_ACCESS, FALSE, deadEventName.c_str());
    if (!IDead) {
        cerr << "Failed to open " << deadEventName << ". Error: " << GetLastError() << endl;
        CloseHandle(NeedToRead);
        CloseHandle(ICanRead);
        CloseHandle(hNamedPipe);
        return 1;
    }
    
    cout << "Client " << threadId << " connected successfully." << endl;
    
    while (true) {
        cout << "q - exit, r - read, s - send: ";
        char choose;
        cin >> choose;
        
        switch (choose) {
        case 'q':
            SetEvent(IDead);
            CleanUp();
            cout << "Exiting client." << endl;
            return 0;

        case 'r':
            message.id = threadId;
            message.type = READ_REQUEST;
            cout << "Enter employee ID you want to read: ";
            cin >> message.employeeId;
            
            WriteFile(
                hNamedPipe, // pipe handle
                &message, // buffer for output
                sizeof(Message), // bytes to write
                &dwBytesWrite, // bytes written
                (LPOVERLAPPED)NULL // synchronous write
            );
            
            SetEvent(NeedToRead);
            WaitForSingleObject(ICanRead, INFINITE);
            ResetEvent(ICanRead);
            
            ReadFile(
                hNamedPipe, // pipe handle
                &message, // buffer for input
                sizeof(Message), // bytes to read
                &dwBytesRead, // bytes read
                (LPOVERLAPPED)NULL // synchronous read
            );
            
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
            break;
            
        case 's':
            message.id = threadId;
            message.type = WRITE_REQUEST;
            cout << "Enter employee ID you want to modify: ";
            cin >> message.employeeId;
            
            WriteFile(
                hNamedPipe, // pipe handle
                &message, // buffer for output
                sizeof(Message), // bytes to write
                &dwBytesWrite, // bytes written
                (LPOVERLAPPED)NULL // synchronous write
            );
            
            SetEvent(NeedToRead);
            WaitForSingleObject(ICanRead, INFINITE);
            ResetEvent(ICanRead);
            
            ReadFile(
                hNamedPipe, // pipe handle
                &message, // buffer for input
                sizeof(Message), // bytes to read
                &dwBytesRead, // bytes read
                (LPOVERLAPPED)NULL // synchronous read
            );
            
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
                message.id = threadId;
                
                WriteFile(
                    hNamedPipe, // pipe handle
                    &message, // buffer for output
                    sizeof(Message), // bytes to write
                    &dwBytesWrite, // bytes written
                    (LPOVERLAPPED)NULL // synchronous write
                );
                
                SetEvent(NeedToRead);
                WaitForSingleObject(ICanRead, INFINITE);
                ResetEvent(ICanRead);
                
                ReadFile(
                    hNamedPipe, // pipe handle
                    &message, // buffer for input
                    sizeof(Message), // bytes to read
                    &dwBytesRead, // bytes read
                    (LPOVERLAPPED)NULL // synchronous read
                );
                
                if (message.type == SUCCESS) {
                    cout << "Record successfully updated:" << endl;
                    DisplayEmployee(message.employee);
                }
                else {
                    cout << "Failed to update record." << endl;
                }
            }
            break;
            
        default:
            cout << "Invalid option. Please try again." << endl;
            break;
        }
    }
    
    // This should never be reached, but just in case
    CleanUp();
    return 0;
}