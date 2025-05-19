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
HANDLE NeedToRead;
HANDLE hNamedPipe;
HANDLE* YouCanReadNow;
HANDLE* AllDead;

// Track which records are being accessed and how (true for write, false for read)
vector<pair<int, bool>> accessedRecords;

void CleanUp() {
    for (auto pi : piv) {
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }
    CloseHandle(NeedToRead);
    CloseHandle(hNamedPipe);
    
    for (int i = 0; i < piv.size(); i++) {
        CloseHandle(YouCanReadNow[i]);
        CloseHandle(AllDead[i]);
    }
    
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

    for (int i = 0; i < ClKol; i++) {
        ZeroMemory(&siv[i], sizeof(STARTUPINFO));
        siv[i].cb = sizeof(STARTUPINFO);
    }

    hNamedPipe = CreateNamedPipe(
        "\\\\.\\pipe\\pipe", // pipe name
        PIPE_ACCESS_DUPLEX, // read/write access
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES, // max instances
        0, // default output buffer size
        0, // default input buffer size
        INFINITE, // client waits indefinitely
        (LPSECURITY_ATTRIBUTES)NULL
    );

    string commLine = "Client98.exe ";
    for (int i = 0; i < ClKol; i++) {
        CreateProcess(NULL, strdup((commLine + to_string(i)).c_str()), NULL, NULL, FALSE,
            CREATE_NEW_CONSOLE, NULL, NULL, &siv[i], &piv[i]);
        
        if (ConnectNamedPipe(hNamedPipe, (LPOVERLAPPED)NULL)) {
            cout << "Connected to client " << i << endl;
        }
    }

    NeedToRead = CreateEvent(NULL, false, false, "ReadEvent");
    AllDead = new HANDLE[ClKol];
    for (int i = 0; i < ClKol; i++) {
        AllDead[i] = CreateEvent(NULL, false, false, ("Dead" + to_string(i)).c_str());
    }
    
    YouCanReadNow = new HANDLE[ClKol];
    for (int i = 0; i < ClKol; i++) {
        YouCanReadNow[i] = CreateEvent(NULL, false, false, ("Read" + to_string(i)).c_str());
    }

    while (true) {
        if (WaitForMultipleObjects(ClKol, AllDead, TRUE, 0) != WAIT_TIMEOUT) {
            cout << "All clients have exited" << endl;
            break;
        }

        if (WaitForSingleObject(NeedToRead, 0) != WAIT_TIMEOUT) {
            ResetEvent(NeedToRead);
            DWORD dwBytesRead;
            DWORD dwBytesWrite;
            Message message;
            ReadFile(
                hNamedPipe, // pipe handle
                &message, // buffer for input
                sizeof(Message), // bytes to read
                &dwBytesRead, // bytes read
                (LPOVERLAPPED)NULL // synchronous read
            );

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

            WriteFile(
                hNamedPipe, // pipe handle
                &answ, // buffer for output
                sizeof(Message), // bytes to write
                &dwBytesWrite, // bytes written
                (LPOVERLAPPED)NULL // synchronous write
            );

            SetEvent(YouCanReadNow[message.id]);
        }
    }
    
    DisplayEmployeeFile();
    cout << "Press Enter to close...";
    cin.ignore();
    cin.get();
    CleanUp();
    
    return 0;
}