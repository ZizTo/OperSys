#include "structs.h"
#include <vector>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <thread> // For std::thread (or use Windows API CreateThread)
#include <mutex>
#include <map>
#include <set>
#include <algorithm> // For std::remove

// Global server settings
std::string g_binFileName;
int g_numClients = 0;

// Handles for client events (server side)
HANDLE* g_pClientDeadEvents = nullptr; // Array of events, one for each client to signal exit
HANDLE* g_pYouCanReadNowEvents = nullptr; // Array of events, server signals client it can read response

// Per-record locking mechanism
struct RecordLockInfo {
    bool isWriteLocked = false;
    int writerClientId = -1; // Client ID holding the write lock
    std::set<int> readerClientIds; // Client IDs holding read locks
};
std::map<int, RecordLockInfo> g_recordLocks; // Key: employeeId
std::mutex g_recordLocksMutex; // Mutex to protect g_recordLocks

// --- Forward Declarations ---
void DisplayEmployeeFile();
Employee FindEmployee(int employeeId);
bool ChangeEmployee(const Employee& empData);
DWORD WINAPI ClientHandlerThread(LPVOID lpParam);

struct ClientThreadArgs {
    HANDLE hPipe = INVALID_HANDLE_VALUE;
    // No need to pass clientId, thread will discover it from the first message
};

void CleanupServerResources() {
    if (g_pClientDeadEvents) {
        for (int i = 0; i < g_numClients; ++i) {
            if (g_pClientDeadEvents[i] != NULL) CloseHandle(g_pClientDeadEvents[i]);
        }
        delete[] g_pClientDeadEvents;
        g_pClientDeadEvents = nullptr;
    }
    if (g_pYouCanReadNowEvents) {
        for (int i = 0; i < g_numClients; ++i) {
            if (g_pYouCanReadNowEvents[i] != NULL) CloseHandle(g_pYouCanReadNowEvents[i]);
        }
        delete[] g_pYouCanReadNowEvents;
        g_pYouCanReadNowEvents = nullptr;
    }
    // Other handles (client process/thread handles stored in main) should also be closed.
}

void DisplayEmployee(const Employee& emp) {
    std::cout << "Employee#" << emp.num << " - "
        << emp.name << " - "
        << emp.hours << std::endl;
}

void DisplayEmployeeFile() {
    std::ifstream file(g_binFileName, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for display: " << g_binFileName << std::endl;
        return;
    }
    Employee data;
    std::cout << "\n--- Current Employee File Content ---" << std::endl;
    // Correctly read until the end of file [Corrects extra record bug]
    while (file.read(reinterpret_cast<char*>(&data), sizeof(Employee))) {
        DisplayEmployee(data);
    }
    std::cout << "--- End of File Content ---\n" << std::endl;
    file.close();
}

Employee FindEmployee(int employeeId) {
    std::ifstream file(g_binFileName, std::ios::binary);
    Employee data;
    data.num = -1; // Default to not found

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for finding employee: " << g_binFileName << std::endl;
        return data;
    }

    while (file.read(reinterpret_cast<char*>(&data), sizeof(Employee))) {
        if (data.num == employeeId) {
            file.close();
            return data;
        }
    }
    file.close();
    data.num = -1; // Ensure not found if loop finishes
    return data;
}

bool ChangeEmployee(const Employee& empData) {
    std::fstream file(g_binFileName, std::ios::binary | std::ios::in | std::ios::out);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for modification: " << g_binFileName << std::endl;
        return false;
    }

    Employee tempEmp;
    std::streamoff recordPosition = -1;
    std::streamoff currentPosition = 0;

    // Find the position of the record
    while (file.read(reinterpret_cast<char*>(&tempEmp), sizeof(Employee))) {
        if (tempEmp.num == empData.num) {
            recordPosition = currentPosition;
            break;
        }
        currentPosition = file.tellg();
    }

    if (recordPosition != -1) {
        file.seekp(recordPosition, std::ios::beg);
        if (file.fail()) {
            std::cerr << "Error seeking in file to modify record ID: " << empData.num << std::endl;
            file.close();
            return false;
        }
        file.write(reinterpret_cast<const char*>(&empData), sizeof(Employee));
        if (file.fail()) {
            std::cerr << "Error writing modified record ID: " << empData.num << std::endl;
            file.close();
            return false;
        }
        std::cout << "Server: Record ID " << empData.num << " modified successfully." << std::endl;
    }
    else {
        std::cerr << "Error: Record ID " << empData.num << " not found for modification." << std::endl;
        file.close();
        return false;
    }

    file.close();
    return true;
}


int main() {
    std::cout << "Enter binary file name: ";
    std::cin >> g_binFileName;

    int numEmployees;
    std::cout << "Enter number of employees to create: ";
    std::cin >> numEmployees;

    std::ofstream outFile(g_binFileName, std::ios::binary | std::ios::trunc);
    if (!outFile.is_open()) {
        std::cerr << "Error creating file: " << g_binFileName << std::endl;
        return 1;
    }

    for (int i = 0; i < numEmployees; ++i) {
        Employee emp;
        emp.num = i; // Assign unique ID
        std::cout << "Enter name for employee #" << i << ": ";
        std::cin >> emp.name; // Simplified input, ensure it fits char name[10]
        std::cout << "Enter hours for employee #" << i << ": ";
        std::cin >> emp.hours;
        outFile.write(reinterpret_cast<char*>(&emp), sizeof(Employee));
    }
    outFile.close();

    std::cout << "\nInitial file created." << std::endl;
    DisplayEmployeeFile(); // [cite: 5]

    std::cout << "Enter number of client processes to launch: ";
    std::cin >> g_numClients;

    // Initialize event arrays
    g_pClientDeadEvents = new HANDLE[g_numClients];
    g_pYouCanReadNowEvents = new HANDLE[g_numClients];

    for (int i = 0; i < g_numClients; ++i) {
        std::string deadEventName = "DeadEventClient" + std::to_string(i);
        g_pClientDeadEvents[i] = CreateEvent(NULL, TRUE, FALSE, deadEventName.c_str()); // Manual-reset

        std::string readReadyEventName = "YouCanReadNowClient" + std::to_string(i);
        g_pYouCanReadNowEvents[i] = CreateEvent(NULL, FALSE, FALSE, readReadyEventName.c_str()); // Auto-reset
    }

    std::vector<PROCESS_INFORMATION> clientProcessInfo(g_numClients);
    std::vector<STARTUPINFO> clientStartupInfo(g_numClients);

    // Launch client processes [cite: 6]
    for (int i = 0; i < g_numClients; ++i) {
        ZeroMemory(&clientStartupInfo[i], sizeof(STARTUPINFO));
        clientStartupInfo[i].cb = sizeof(STARTUPINFO);
        ZeroMemory(&clientProcessInfo[i], sizeof(PROCESS_INFORMATION));

        std::string commandLine = "Client98.exe " + std::to_string(i); // Assuming client executable is named client.exe

        // CreateProcess needs a non-const char* for command line
        char* cl = new char[commandLine.length() + 1];
        strcpy_s(cl, commandLine.length() + 1, commandLine.c_str());

        if (!CreateProcess(NULL, cl, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &clientStartupInfo[i], &clientProcessInfo[i])) {
            std::cerr << "Failed to create client process #" << i << ". Error: " << GetLastError() << std::endl;
            delete[] cl;
            // Consider cleanup and exit or trying to continue
            continue;
        }
        delete[] cl;
        std::cout << "Server: Launched client process #" << i << " with PID " << clientProcessInfo[i].dwProcessId << std::endl;
    }

    std::vector<HANDLE> clientHandlerThreadHandles;
    clientHandlerThreadHandles.reserve(g_numClients);

    // Main loop to accept client connections
    for (int i = 0; i < g_numClients; ++i) {
        HANDLE hPipe = CreateNamedPipe(
            "\\\\.\\pipe\\employeeDataPipe", // Consistent pipe name
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES, // Allows multiple clients to use this name
            sizeof(Message),          // Output buffer size
            sizeof(Message),          // Input buffer size
            0,                        // Default timeout
            NULL);                    // Default security attributes

        if (hPipe == INVALID_HANDLE_VALUE) {
            std::cerr << "CreateNamedPipe failed for client connection " << i << ", GLE=" << GetLastError() << std::endl;
            // This is serious, might need to break or cleanup existing
            continue;
        }
        std::cout << "Server: Pipe instance created. Waiting for client #" << i << " to connect..." << std::endl;

        // Wait for a client to connect to this pipe instance
        BOOL fConnected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

        if (fConnected) {
            std::cout << "Server: Client #" << i << " connected to pipe instance." << std::endl;
            ClientThreadArgs* pArgs = new ClientThreadArgs();
            pArgs->hPipe = hPipe; // Pass the connected pipe handle to the thread

            HANDLE hThread = CreateThread(
                NULL,              // Default security attributes
                0,                 // Default stack size
                ClientHandlerThread, // Thread function
                pArgs,             // Argument to thread function
                0,                 // Default creation flags
                NULL);             // Don't need thread identifier

            if (hThread == NULL) {
                std::cerr << "CreateThread failed for client " << i << ", GLE=" << GetLastError() << std::endl;
                CloseHandle(hPipe); // Close this pipe instance as thread creation failed
                delete pArgs;
            }
            else {
                clientHandlerThreadHandles.push_back(hThread);
            }
        }
        else {
            std::cerr << "ConnectNamedPipe failed for client " << i << ", GLE=" << GetLastError() << std::endl;
            CloseHandle(hPipe); // Close this pipe instance as connection failed
        }
    }

    std::cout << "Server: All expected clients (" << g_numClients << ") have had connection attempts. Monitoring active clients." << std::endl;

    // Wait for all clients to signal they are dead
    if (g_numClients > 0 && g_pClientDeadEvents != nullptr) {
        std::cout << "Server: Waiting for all " << g_numClients << " client(s) to terminate..." << std::endl;
        WaitForMultipleObjects(g_numClients, g_pClientDeadEvents, TRUE, INFINITE); // TRUE = wait for all
        std::cout << "Server: All clients have terminated." << std::endl;
    }
    else if (g_numClients == 0) {
        std::cout << "Server: No clients were configured to run." << std::endl;
    }


    // Wait for all handler threads to complete
    if (!clientHandlerThreadHandles.empty()) {
        std::cout << "Server: Waiting for all client handler threads to complete..." << std::endl;
        WaitForMultipleObjects(static_cast<DWORD>(clientHandlerThreadHandles.size()), clientHandlerThreadHandles.data(), TRUE, INFINITE);
        std::cout << "Server: All client handler threads completed." << std::endl;
    }
    for (HANDLE h : clientHandlerThreadHandles) {
        CloseHandle(h);
    }
    clientHandlerThreadHandles.clear();


    std::cout << "\nFinal file content after client operations:" << std::endl;
    DisplayEmployeeFile(); // [cite: 9]

    // Close client process handles
    for (int i = 0; i < g_numClients; ++i) {
        if (clientProcessInfo[i].hProcess != NULL) CloseHandle(clientProcessInfo[i].hProcess);
        if (clientProcessInfo[i].hThread != NULL) CloseHandle(clientProcessInfo[i].hThread);
    }

    CleanupServerResources();

    std::cout << "\nServer shutting down. Press Enter to exit." << std::endl; // [cite: 10] (simplified)
    std::cin.ignore(); // Clear leftover newline
    std::cin.get();

    return 0;
}


DWORD WINAPI ClientHandlerThread(LPVOID lpParam) {
    ClientThreadArgs* args = static_cast<ClientThreadArgs*>(lpParam);
    HANDLE hPipe = args->hPipe;
    // Note: The client's ID (0 to g_numClients-1) will come from message.clientId.

    Message request, response;
    DWORD bytesRead, bytesWritten;
    bool clientActive = true;

    std::cout << "Server Thread: Handler started for a client. Pipe handle: " << hPipe << std::endl;

    while (clientActive) {
        // Read a request from the client
        BOOL fSuccess = ReadFile(
            hPipe,
            &request,
            sizeof(Message),
            &bytesRead,
            NULL);

        if (!fSuccess || bytesRead == 0) {
            if (GetLastError() == ERROR_BROKEN_PIPE) {
                std::cout << "Server Thread: Client disconnected (broken pipe)." << std::endl;
            }
            else {
                std::cout << "Server Thread: ReadFile failed, GLE=" << GetLastError() << std::endl;
            }
            clientActive = false; // Assume client exited
            break;
        }

        // If client sends an explicit exit message type, handle it.
        // (Currently, client signals exit via its "Dead" event, not a pipe message)

        std::cout << "Server Thread: Received request type " << request.type << " from client ID " << request.clientId << " for employee ID " << request.employeeId << std::endl;

        response.clientId = request.clientId;
        response.employeeId = request.employeeId;

        // Lock the global record lock structure before accessing
        std::lock_guard<std::mutex> guard(g_recordLocksMutex);

        RecordLockInfo& lockInfo = g_recordLocks[request.employeeId]; // Creates if not exists

        switch (request.type) {
        case READ_REQUEST: // [cite: 8]
            if (lockInfo.isWriteLocked) { // Block if write locked by anyone [cite: 9]
                response.type = BLOCK_RESPONSE;
                std::cout << "Server Thread: READ_REQUEST for emp " << request.employeeId << " blocked by write lock held by client " << lockInfo.writerClientId << std::endl;
            }
            else {
                response.employee = FindEmployee(request.employeeId);
                if (response.employee.num == -1) {
                    response.type = FAIL_READ;
                    std::cout << "Server Thread: READ_REQUEST for emp " << request.employeeId << " failed (not found)." << std::endl;
                }
                else {
                    response.type = SUCCESS_READ;
                    lockInfo.readerClientIds.insert(request.clientId); // Add this client to readers
                    std::cout << "Server Thread: READ_REQUEST for emp " << request.employeeId << " succeeded. Readers: " << lockInfo.readerClientIds.size() << std::endl;
                }
            }
            break;

        case WRITE_REQUEST: // [cite: 7]
            // Block if write locked by another, or if any read locks exist [cite: 7, 9]
            if (lockInfo.isWriteLocked || !lockInfo.readerClientIds.empty()) {
                response.type = BLOCK_RESPONSE;
                std::cout << "Server Thread: WRITE_REQUEST for emp " << request.employeeId << " blocked. Write locked: " << lockInfo.isWriteLocked << ", Readers: " << lockInfo.readerClientIds.size() << std::endl;
            }
            else {
                response.employee = FindEmployee(request.employeeId);
                if (response.employee.num == -1) {
                    response.type = FAIL_READ;
                    std::cout << "Server Thread: WRITE_REQUEST for emp " << request.employeeId << " failed (not found)." << std::endl;
                }
                else {
                    response.type = SUCCESS_READ; // Indicates client can proceed to modify
                    lockInfo.isWriteLocked = true;
                    lockInfo.writerClientId = request.clientId;
                    std::cout << "Server Thread: WRITE_REQUEST for emp " << request.employeeId << " granted. Write lock acquired by client " << request.clientId << std::endl;
                }
            }
            break;

        case WRITE_REQUEST_READY:
            if (lockInfo.isWriteLocked && lockInfo.writerClientId == request.clientId) {
                if (ChangeEmployee(request.employee)) {
                    response.type = SUCCESS;
                    response.employee = FindEmployee(request.employee.num); // Send back the confirmed state
                    std::cout << "Server Thread: WRITE_REQUEST_READY for emp " << request.employee.num << " by client " << request.clientId << " succeeded." << std::endl;
                }
                else {
                    response.type = FAIL_READ; // Generic fail for write operation
                    std::cout << "Server Thread: WRITE_REQUEST_READY for emp " << request.employee.num << " by client " << request.clientId << " failed during ChangeEmployee." << std::endl;
                }
                // Release write lock
                lockInfo.isWriteLocked = false;
                lockInfo.writerClientId = -1;
            }
            else {
                // Client might have lost the lock or was not the owner
                response.type = BLOCK_RESPONSE; // Or a more specific error type
                std::cout << "Server Thread: WRITE_REQUEST_READY for emp " << request.employee.num << " by client " << request.clientId << " denied (not lock owner or no lock)." << std::endl;
            }
            break;

            // Default case for unknown message types could be added
        default:
            std::cerr << "Server Thread: Unknown message type " << request.type << " from client " << request.clientId << std::endl;
            response.type = FAIL_READ; // Generic error
            break;
        }
        // Unlock guard (automatically by going out of scope)
        // g_recordLocksMutex.unlock(); // Not needed with lock_guard

        // Send the response to the client
        fSuccess = WriteFile(
            hPipe,
            &response,
            sizeof(Message),
            &bytesWritten,
            NULL);

        if (!fSuccess) {
            std::cout << "Server Thread: WriteFile failed for client " << request.clientId << ", GLE=" << GetLastError() << std::endl;
            clientActive = false; // Assume client exited
        }
        else {
            std::cout << "Server Thread: Sent response type " << response.type << " to client " << request.clientId << std::endl;
        }

        // Signal the client that it can now read the response it's waiting for
        // This matches the client's expectation: Write, SetEvent(NeedToRead), WaitFor(ICanRead), Read.
        // The server thread has Read, Process, Write (above), then SetEvent(YouCanReadNow)
        if (request.clientId >= 0 && request.clientId < g_numClients && g_pYouCanReadNowEvents && g_pYouCanReadNowEvents[request.clientId]) {
            SetEvent(g_pYouCanReadNowEvents[request.clientId]);
        }
        else {
            std::cerr << "Server Thread: Invalid client ID " << request.clientId << " or events not initialized for signaling." << std::endl;
        }
    }

    // Client session ended (disconnected or error)
    // Clean up any locks held by this client
    std::lock_guard<std::mutex> guard(g_recordLocksMutex);
    for (auto& pair : g_recordLocks) {
        RecordLockInfo& lockInfo = pair.second;
        if (lockInfo.writerClientId == request.clientId) { // Use last known request.clientId if valid
            lockInfo.isWriteLocked = false;
            lockInfo.writerClientId = -1;
            std::cout << "Server Thread: Cleaned up write lock for emp " << pair.first << " held by client " << request.clientId << std::endl;
        }
        lockInfo.readerClientIds.erase(request.clientId);
        if (lockInfo.readerClientIds.count(request.clientId)) { // Check before erase
            lockInfo.readerClientIds.erase(request.clientId);
            std::cout << "Server Thread: Cleaned up read lock for emp " << pair.first << " held by client " << request.clientId << std::endl;
        }
    }
    // g_recordLocksMutex.unlock(); // Not needed with lock_guard

    std::cout << "Server Thread: Handler for client (last known ID: " << request.clientId << ") is terminating. Pipe handle: " << hPipe << std::endl;
    FlushFileBuffers(hPipe);
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);
    delete args; // Clean up allocated ClientThreadArgs
    return 0;
}