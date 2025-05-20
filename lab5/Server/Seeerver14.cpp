#include "structs.h"
#include <vector>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#include <map>
#include <set>
#include <algorithm>

using namespace std;

string binFl;
int clKol = 0;

HANDLE* allDeadEvents = nullptr;
HANDLE* YouReadNowEvents = nullptr;

struct RecordLockInfo {
    bool isWriteLocked = false;
    int writerClientId = -1;
    set<int> readerClientIds;
};

map<int, RecordLockInfo> recLocks;
mutex recLocksMutex;

void DisplayEmployeeFile();
Employee FindEmployee(int employeeId);
bool ChangeEmployee(const Employee& empData);
DWORD WINAPI ClientHandlerThread(LPVOID lpParam);

struct ClientThreadArgs {
    HANDLE hPipe = INVALID_HANDLE_VALUE;
};

void CleanUp() {
    for (int i = 0; i < clKol; ++i) {
        if (allDeadEvents[i] != NULL) CloseHandle(allDeadEvents[i]);
    }
    delete[] allDeadEvents;
    for (int i = 0; i < clKol; ++i) {
        if (YouReadNowEvents[i] != NULL) CloseHandle(YouReadNowEvents[i]);
    }
    delete[] YouReadNowEvents;
}

void DisplayEmployee(const Employee& emp) {
    cout << "Employee#" << emp.num << " - "
        << emp.name << " - "
        << emp.hours << endl;
}

void DisplayEmployeeFile() {
    ifstream file(binFl, ios::binary);
    if (!file.is_open()) {
        cerr << "Cant open " << binFl << endl;
        return;
    }
    Employee data;
    cout << endl << "--- Employee file ---" << endl;
    while (file.read(reinterpret_cast<char*>(&data), sizeof(Employee))) {
        DisplayEmployee(data);
    }
    cout << endl << endl;
    file.close();
}

Employee FindEmployee(int employeeId) {
    ifstream file(binFl, ios::binary);
    Employee data;
    data.num = -1;

    if (!file.is_open()) {
        cerr << "Cant open " << binFl << endl;
        return data;
    }

    while (file.read(reinterpret_cast<char*>(&data), sizeof(Employee))) {
        if (data.num == employeeId) {
            file.close();
            return data;
        }
    }
    file.close();
    data.num = -1;
    return data;
}

bool ChangeEmployee(const Employee& empData) {
    fstream file(binFl, ios::binary | ios::in | ios::out);
    if (!file.is_open()) {
        cerr << "Cant open " << binFl << endl;
        return false;
    }

    Employee tempEmp;
    streamoff recordPosition = -1;
    streamoff currentPosition = 0;

    while (file.read(reinterpret_cast<char*>(&tempEmp), sizeof(Employee))) {
        if (tempEmp.num == empData.num) {
            recordPosition = currentPosition;
            break;
        }
        currentPosition = file.tellg();
    }

    if (recordPosition != -1) {
        file.seekp(recordPosition, ios::beg);
        file.write(reinterpret_cast<const char*>(&empData), sizeof(Employee));
        cout << "Modified " << empData.num << endl;
    }
    else {
        cerr << "no " << empData.num << " in file" << endl;
        file.close();
        return false;
    }

    file.close();
    return true;
}


int main() {
    cout << "Bin file name: ";
    cin >> binFl;

    int numEmpl;
    cout << "Num of employees: ";
    cin >> numEmpl;

    ofstream outFile(binFl, ios::binary | ios::trunc);
    if (!outFile.is_open()) {
        cerr << "file err " << binFl << endl;
        return 1;
    }

    for (int i = 0; i < numEmpl; ++i) {
        Employee emp;
        emp.num = i;
        cout << "Enter name for employee #" << i << ": ";
        cin >> emp.name;
        cout << "Enter hours for employee #" << i << ": ";
        cin >> emp.hours;
        outFile.write(reinterpret_cast<char*>(&emp), sizeof(Employee));
    }
    outFile.close();

    DisplayEmployeeFile();

    cout << "Number of clients: ";
    cin >> clKol;

    allDeadEvents = new HANDLE[clKol];
    YouReadNowEvents = new HANDLE[clKol];

    for (int i = 0; i < clKol; ++i) {
        string deadEventName = "DeadEventClient" + to_string(i);
        allDeadEvents[i] = CreateEvent(NULL, TRUE, FALSE, deadEventName.c_str());

        string readReadyEventName = "YouCanReadNowClient" + to_string(i);
        YouReadNowEvents[i] = CreateEvent(NULL, FALSE, FALSE, readReadyEventName.c_str());
    }

    vector<PROCESS_INFORMATION> clPi(clKol);
    vector<STARTUPINFO> clSi(clKol);

    for (int i = 0; i < clKol; ++i) {
        ZeroMemory(&clSi[i], sizeof(STARTUPINFO));
        clSi[i].cb = sizeof(STARTUPINFO);
        ZeroMemory(&clPi[i], sizeof(PROCESS_INFORMATION));

        string commandLine = "Client98.exe " + to_string(i); 

        char* cl = new char[commandLine.length() + 1];
        strcpy_s(cl, commandLine.length() + 1, commandLine.c_str());

        if (!CreateProcess(NULL, cl, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &clSi[i], &clPi[i])) {
            cerr << "Client create err #" << i << ": " << GetLastError() << endl;
            delete[] cl;
            continue;
        }
        delete[] cl;
        cout << "started process #" << i << endl;
    }

    vector<HANDLE> clhThreadH;
    clhThreadH.reserve(clKol);

    for (int i = 0; i < clKol; ++i) {
        HANDLE hPipe = CreateNamedPipe(
            "\\\\.\\pipe\\employeeDataPipe",
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES,
            sizeof(Message),          
            sizeof(Message),          
            0,                        
            NULL);                    

        if (hPipe == INVALID_HANDLE_VALUE) {
            cerr << "Pipe err " << i << endl;
            continue;
        }

        if (ConnectNamedPipe(hPipe, NULL)) {
            cout << "Client #" << i << " connected" << endl;
            ClientThreadArgs* pArgs = new ClientThreadArgs();
            pArgs->hPipe = hPipe;
            HANDLE hThread = CreateThread(
                NULL,
                0,
                ClientHandlerThread,
                pArgs,
                0,
                NULL);
            clhThreadH.push_back(hThread);
        }
        else {
            cerr << "CNP err " << i << endl;
            CloseHandle(hPipe);
        }
    }

    if (clKol > 0) {
        WaitForMultipleObjects(clKol, allDeadEvents, TRUE, INFINITE);
        cout << "All clients dead" << endl;
    }
    else if (clKol == 0) {
        cout << "No clients" << endl;
    }
    WaitForMultipleObjects(static_cast<DWORD>(clhThreadH.size()), clhThreadH.data(), TRUE, INFINITE);
    for (HANDLE h : clhThreadH) {
        CloseHandle(h);
    }
    clhThreadH.clear();

    DisplayEmployeeFile();

    for (int i = 0; i < clKol; ++i) {
        if (clPi[i].hProcess != NULL) CloseHandle(clPi[i].hProcess);
        if (clPi[i].hThread != NULL) CloseHandle(clPi[i].hThread);
    }

    CleanUp();

    cout << endl << "press to close..." << endl;
    cin.ignore();
    cin.get();

    return 0;
}


DWORD WINAPI ClientHandlerThread(LPVOID lpParam) {
    ClientThreadArgs* args = static_cast<ClientThreadArgs*>(lpParam);
    HANDLE hPipe = args->hPipe;

    Message request, response;
    DWORD bytesRead, bytesWritten;
    bool clientActive = true;

    while (clientActive) {
        bool fSuccess = ReadFile(
            hPipe,
            &request,
            sizeof(Message),
            &bytesRead,
            NULL);

        if (!fSuccess || bytesRead == 0) {
            clientActive = false;
        }
        cout << "Got " << request.type << " from " << request.clientId << endl;

        response.clientId = request.clientId;
        response.employeeId = request.employeeId;

        lock_guard<mutex> guard(recLocksMutex);

        RecordLockInfo& lockInfo = recLocks[request.employeeId];

        switch (request.type) {
        case READ_REQUEST:
            if (lockInfo.isWriteLocked) {
                response.type = BLOCK_RESPONSE;
            }
            else {
                response.employee = FindEmployee(request.employeeId);
                if (response.employee.num == -1) {
                    response.type = FAIL_READ;
                }
                else {
                    response.type = SUCCESS_READ;
                    lockInfo.readerClientIds.insert(request.clientId);
                }
            }
            break;

        case WRITE_REQUEST:
            if (lockInfo.isWriteLocked || !lockInfo.readerClientIds.empty()) {
                response.type = BLOCK_RESPONSE;
            }
            else {
                response.employee = FindEmployee(request.employeeId);
                if (response.employee.num == -1) {
                    response.type = FAIL_READ;
                }
                else {
                    response.type = SUCCESS_READ;
                    lockInfo.isWriteLocked = true;
                    lockInfo.writerClientId = request.clientId;
                }
            }
            break;

        case WRITE_REQUEST_READY:
            if (lockInfo.isWriteLocked && lockInfo.writerClientId == request.clientId) {
                if (ChangeEmployee(request.employee)) {
                    response.type = SUCCESS;
                    response.employee = FindEmployee(request.employee.num);
                }
                else {
                    response.type = FAIL_READ; 
                }
                lockInfo.isWriteLocked = false;
                lockInfo.writerClientId = -1;
            }
            else {
                response.type = BLOCK_RESPONSE;
            }
            break;

        default:
            response.type = FAIL_READ;
            break;
        }
        WriteFile(
            hPipe,
            &response,
            sizeof(Message),
            &bytesWritten,
            NULL);

        SetEvent(YouReadNowEvents[request.clientId]);
    }

    lock_guard<mutex> guard(recLocksMutex);

    for (auto& pair : recLocks) {
        RecordLockInfo& lockInfo = pair.second;
        if (lockInfo.writerClientId == request.clientId) {
            lockInfo.isWriteLocked = false;
            lockInfo.writerClientId = -1;
        }
        lockInfo.readerClientIds.erase(request.clientId);
    }

    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);
    delete args;
    return 0;
}