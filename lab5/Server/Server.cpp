#include "structs.h"
#include <vector>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

using namespace std;

string binFl;

vector<STARTUPINFO> siv;
vector<PROCESS_INFORMATION> piv;
vector<HANDLE> hPipes;  // Array of pipe handles, one for each client
HANDLE* NeedToRead;     // Array of events for client read requests
HANDLE* YouCanReadNow;  // Array of events for server responses
HANDLE* AllDead;        // Array of events for client termination

// Track which records are being accessed and how (true for write, false for read)
vector<pair<int, bool>> accessedRecords;

void CleanUp() {
    for (auto pi : piv) {
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }
    
    for (int i = 0; i < piv.size(); i++) {
        CloseHandle(NeedToRead[i]);
        CloseHandle(YouCanReadNow[i]);
        CloseHandle(AllDead[i]);
        CloseHandle(hPipes[i]);
    }
    
    delete[] NeedToRead;
    delete[] YouCanReadNow;
    delete[] AllDead;
}

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

bool isRecordAccessedForWrite(int employeeId) {
    for (const auto& rec : accessedRecords) {
        if (rec.first == employeeId && rec.second == true) {
            return true;
        }
    }
    return false;
}

bool isRecordAccessed(int employeeId) {
    return std::any_of(accessedRecords.begin(), accessedRecords.end(),
        [employeeId](const pair<int, bool>& rec) { return rec.first == employeeId; });
}

void removeRecordAccess(int employeeId) {
    accessedRecords.erase(
        std::remove_if(accessedRecords.begin(), accessedRecords.end(),
            [employeeId](const pair<int, bool>& rec) { return rec.first == employeeId; }),
        accessedRecords.end());
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

    cout << "Client count: ";
    int ClKol;
    cin >> ClKol;
    piv.resize(ClKol);
    siv.resize(ClKol);
    hPipes.resize(ClKol);
    
    // Create events for each client
    NeedToRead = new HANDLE[ClKol];
    YouCanReadNow = new HANDLE[ClKol];
    AllDead = new HANDLE[ClKol];

    // Initialize each client's events
    for (int i = 0; i < ClKol; i++) {
        string readEventName = "ReadEvent" + to_string(i);
        string responseEventName = "Read" + to_string(i);
        string deadEventName = "Dead" + to_string(i);
        
        NeedToRead[i] = CreateEvent(NULL, false, false, readEventName.c_str());
        YouCanReadNow[i] = CreateEvent(NULL, false, false, responseEventName.c_str());
        AllDead[i] = CreateEvent(NULL, false, false, deadEventName.c_str());
        
        ZeroMemory(&siv[i], sizeof(STARTUPINFO));
        siv[i].cb = sizeof(STARTUPINFO);
    }

    // Create and connect to each client with a unique pipe
    for (int i = 0; i < ClKol; i++) {
        string pipeName = "\\\\.\\pipe\\pipe" + to_string(i);
        
        // Create a named pipe for this client
        hPipes[i] = CreateNamedPipe(
            pipeName.c_str(),             // unique pipe name for each client
            PIPE_ACCESS_DUPLEX,           // read/write access
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            1,                            // only one instance per pipe
            0,                            // default output buffer size
            0,                            // default input buffer size
            INFINITE,                     // client waits indefinitely
            (LPSECURITY_ATTRIBUTES)NULL
        );
        
        if (hPipes[i] == INVALID_HANDLE_VALUE) {
            cerr << "CreateNamedPipe failed for client " << i << ". Error: " << GetLastError() << endl;
            continue;
        }
        
        // Create the client process - IMPORTANT: Make sure the client executable name matches exactly
        // Note: The client executable should be in the current directory or use full path
        string commLine = "Client.exe " + to_string(i);  // Changed from Client98.exe to Client.exe
        cout << "Launching: " << commLine << endl;
        
        if (!CreateProcess(NULL, (LPSTR)commLine.c_str(), NULL, NULL, FALSE,
                          CREATE_NEW_CONSOLE, NULL, NULL, &siv[i], &piv[i])) {
            cerr << "CreateProcess failed for client " << i << ". Error: " << GetLastError() << endl;
            CloseHandle(hPipes[i]);
            continue;
        }
        
        // Wait for the client to connect to the pipe
        cout << "Waiting for client " << i << " to connect..." << endl;
        if (!ConnectNamedPipe(hPipes[i], NULL)) {
            DWORD error = GetLastError();
            if (error != ERROR_PIPE_CONNECTED) {  // Ignore if already connected
                cerr << "ConnectNamedPipe failed for client " << i << ". Error: " << error << endl;
                CloseHandle(hPipes[i]);
                continue;
            }
        }
        
        cout << "Client " << i << " connected successfully." << endl;
    }

    // Main server loop to handle client requests
    DWORD dwWaitResult;
    HANDLE* allEvents = new HANDLE[ClKol];
    bool clientsActive = true;
    
    while (clientsActive) {
        // Check if all clients have terminated
        if (WaitForMultipleObjects(ClKol, AllDead, TRUE, 0) != WAIT_TIMEOUT) {
            cout << "All clients have exited" << endl;
            break;
        }
        
        // Copy all read request events to an array for WaitForMultipleObjects
        for (int i = 0; i < ClKol; i++) {
            allEvents[i] = NeedToRead[i];
        }
        
        // Wait for any client to signal a request
        dwWaitResult = WaitForMultipleObjects(ClKol, allEvents, FALSE, 100);
        
        // Check if a client has a request (not timeout)
        if (dwWaitResult != WAIT_TIMEOUT && dwWaitResult < WAIT_OBJECT_0 + ClKol) {
            int clientIndex = dwWaitResult - WAIT_OBJECT_0;
            ResetEvent(NeedToRead[clientIndex]);
            
            // Process the client's request
            DWORD dwBytesRead;
            DWORD dwBytesWrite;
            Message message;
            
            if (!ReadFile(
                hPipes[clientIndex],  // pipe for this client
                &message,             // buffer for input
                sizeof(Message),      // bytes to read
                &dwBytesRead,         // bytes read
                NULL                  // synchronous read
            )) {
                cerr << "ReadFile failed for client " << clientIndex << ". Error: " << GetLastError() << endl;
                continue;
            }
            
            Message answ;
            answ.id = message.id;

            switch (message.type) {
            case READ_REQUEST:
                if (!isRecordAccessedForWrite(message.employeeId)) {
                    // Record is not being written to, can read
                    answ.employee = FindEmployee(message.employeeId);
                    if (answ.employee.num == -1) {
                        answ.type = FAIL_READ;
                    } else {
                        accessedRecords.push_back(make_pair(message.employeeId, false)); // Mark as being read
                        answ.type = SUCCESS_READ;
                    }
                } else {
                    // Record is being written to, can't read
                    answ.type = BLOCK_RESPONSE;
                }
                break;
                
            case WRITE_REQUEST:
                if (!isRecordAccessed(message.employeeId)) {
                    // Record is not being accessed at all, can write
                    answ.employee = FindEmployee(message.employeeId);
                    if (answ.employee.num == -1) {
                        answ.type = FAIL_READ;
                    } else {
                        accessedRecords.push_back(make_pair(message.employeeId, true)); // Mark as being written
                        answ.type = SUCCESS_READ;
                    }
                } else {
                    // Record is being accessed, can't write
                    answ.type = BLOCK_RESPONSE;
                }
                break;
                
            case WRITE_REQUEST_READY:
                ChangeEmployee(message.employee.num, message.employee);
                answ.type = SUCCESS;
                answ.employee = message.employee; // Send the updated employee back
                
                // Remove the record from access control
                removeRecordAccess(message.employeeId);
                break;

            default:
                cerr << "Unknown message type: " << message.type << endl;
                answ.type = FAIL_READ;
                break;
            }
            
            if (!WriteFile(
                hPipes[clientIndex],  // pipe for this client
                &answ,                // buffer for output
                sizeof(Message),      // bytes to write
                &dwBytesWrite,        // bytes written
                NULL                  // synchronous write
            )) {
                cerr << "WriteFile failed for client " << clientIndex << ". Error: " << GetLastError() << endl;
                continue;
            }
            
            // Signal the client that it can now read the response
            SetEvent(YouCanReadNow[clientIndex]);
        }
    }
    
    delete[] allEvents;
    DisplayEmployeeFile();
    cout << "Press Enter to close...";
    cin.ignore();
    cin.get();
    CleanUp();
    
    return 0;
}