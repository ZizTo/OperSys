#include "structs.h"
// #include <vector> // Not used here
#include <windows.h>
#include <iostream>
// #include <fstream> // Not used here
#include <string>
// #include <vector> // Not used directly

// using namespace std; // Avoid using namespace std globally in .cpp files if possible

HANDLE hNamedPipe = INVALID_HANDLE_VALUE;
HANDLE hNeedToReadEvent = NULL;   // Server's global event that client signals
HANDLE hICanReadEvent = NULL;     // Client's unique event, server signals this
HANDLE hIDeadEvent = NULL;        // Client signals this on exit

int g_clientId = -1; // Store client's unique ID

void CleanUpClient() {
    if (hNeedToReadEvent != NULL) CloseHandle(hNeedToReadEvent);
    if (hICanReadEvent != NULL) CloseHandle(hICanReadEvent);
    if (hIDeadEvent != NULL) CloseHandle(hIDeadEvent);
    if (hNamedPipe != INVALID_HANDLE_VALUE) CloseHandle(hNamedPipe);
    std::cout << "Client #" << g_clientId << ": Cleanup complete." << std::endl;
}

void DisplayEmployee(const Employee& emp) {
    std::cout << "Client #" << g_clientId << ": Employee Details: ID=" << emp.num << ", Name="
        << emp.name << ", Hours="
        << emp.hours << std::endl;
}

Employee ModifyEmployee(Employee data) { // [cite: 14]
    std::cout << "Client #" << g_clientId << ": Enter new name for employee #" << data.num << " (current: " << data.name << "): ";
    std::string newName;
    std::cin >> newName;
    strncpy_s(data.name, sizeof(data.name), newName.c_str(), _TRUNCATE); // Safer copy

    std::cout << "Client #" << g_clientId << ": Enter new hours for employee #" << data.num << " (current: " << data.hours << "): ";
    std::cin >> data.hours;
    while (std::cin.fail()) { // Basic input validation
        std::cout << "Invalid input for hours. Please enter a number: ";
        std::cin.clear();
        std::cin.ignore(256, '\n');
        std::cin >> data.hours;
    }
    return data;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Client Error: Missing client ID argument." << std::endl;
        return 1;
    }
    g_clientId = std::atoi(argv[1]);
    std::cout << "Client #" << g_clientId << " started." << std::endl;

    // Open events created by the server (or that server expects)
    // Event names must match what the server creates/expects
    hNeedToReadEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, "ReadEvent"); // This is the server's global event
    if (hNeedToReadEvent == NULL) {
        std::cerr << "Client #" << g_clientId << ": Failed to open NeedToReadEvent. GLE=" << GetLastError() << std::endl;
        // This event is actually set by client, so server must have created it.
        // If client can't open it, it means server didn't create it or name mismatch.
        // For this lab, client *sets* it. Server *waits* on it.
        // The original client code: NeedToRead = OpenEvent(EVENT_ALL_ACCESS, FALSE, "ReadEvent");
        // Let's assume server created it. If not, the client's SetEvent will fail silently or do nothing.
        // To be robust, client could try to create it if OpenEvent fails, but that depends on design.
        // For now, assume server creates "ReadEvent".
        // Let's revert to OpenEvent for "ReadEvent" and create for client-specific ones.
    }


    std::string iCanReadEventName = "YouCanReadNowClient" + std::to_string(g_clientId); // Matches server's creation
    hICanReadEvent = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, iCanReadEventName.c_str());
    if (hICanReadEvent == NULL) {
        std::cerr << "Client #" << g_clientId << ": Failed to open ICanReadEvent (" << iCanReadEventName << "). GLE=" << GetLastError() << std::endl;
        CleanUpClient();
        return 1;
    }


    std::string iDeadEventName = "DeadEventClient" + std::to_string(g_clientId); // Matches server's creation
    hIDeadEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, iDeadEventName.c_str());
    if (hIDeadEvent == NULL) {
        std::cerr << "Client #" << g_clientId << ": Failed to open IDeadEvent (" << iDeadEventName << "). GLE=" << GetLastError() << std::endl;
        CleanUpClient();
        return 1;
    }

    // Connect to the named pipe
    hNamedPipe = CreateFile(
        "\\\\.\\pipe\\employeeDataPipe",    // Pipe name
        GENERIC_READ | GENERIC_WRITE, // Read and write access
        0,                            // No sharing
        NULL,                         // Default security attributes
        OPEN_EXISTING,                // Opens existing pipe
        0,                            // Default attributes
        NULL);                        // No template file

    if (hNamedPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Client #" << g_clientId << ": Could not connect to pipe. GLE=" << GetLastError() << std::endl;
        CleanUpClient();
        return 1;
    }
    std::cout << "Client #" << g_clientId << ": Connected to named pipe." << std::endl;


    Message message;
    DWORD dwBytesProcessed; // For WriteFile and ReadFile

    bool running = true;
    while (running) {
        std::cout << "\nClient #" << g_clientId << ": Choose operation:" << std::endl;
        std::cout << "  'r' - Read record" << std::endl; // [cite: 11]
        std::cout << "  's' - Modify record" << std::endl; // [cite: 11]
        std::cout << "  'q' - Quit" << std::endl; // [cite: 11]
        std::cout << "Enter choice: ";
        char choice;
        std::cin >> choice;
        std::cin.ignore(256, '\n'); // consume trailing newline

        message.clientId = g_clientId; // Always set the client ID in the message

        switch (choice) {
        case 'q':
            running = false;
            if (hIDeadEvent) SetEvent(hIDeadEvent); // Signal server that this client is exiting
            std::cout << "Client #" << g_clientId << ": Quit command received. Signaling exit." << std::endl;
            break;

        case 'r': // Read operation [cite: 16]
            message.type = READ_REQUEST;
            std::cout << "Client #" << g_clientId << ": Enter employee ID to read: ";
            std::cin >> message.employeeId; // [cite: 16]
            while (std::cin.fail()) {
                std::cout << "Invalid input. Please enter a number for employee ID: ";
                std::cin.clear();
                std::cin.ignore(256, '\n');
                std::cin >> message.employeeId;
            }
            std::cin.ignore(256, '\n');


            // Send request to server [cite: 16]
            if (!WriteFile(hNamedPipe, &message, sizeof(Message), &dwBytesProcessed, NULL)) {
                std::cerr << "Client #" << g_clientId << ": WriteFile (read request) failed. GLE=" << GetLastError() << std::endl;
                running = false; break;
            }

            // The original client sets "NeedToRead" event. This event is for the *server's* attention.
            // However, if server has dedicated threads per client pipe, this global event is less critical for routing.
            // For compatibility with original design intent, we can keep it.
            // if (hNeedToReadEvent) SetEvent(hNeedToReadEvent); // This was in original code.

            // Wait for server to signal that response is ready to be read [matches server's SetEvent(YouCanReadNowEvents[clientId])]
            if (WaitForSingleObject(hICanReadEvent, INFINITE) != WAIT_OBJECT_0) {
                std::cerr << "Client #" << g_clientId << ": WaitForSingleObject (hICanReadEvent) failed. GLE=" << GetLastError() << std::endl;
                running = false; break;
            }
            // ResetEvent(hICanReadEvent); // Server uses auto-reset, so client doesn't need to reset. If server used manual, client would.

            // Read response from server
            if (!ReadFile(hNamedPipe, &message, sizeof(Message), &dwBytesProcessed, NULL) || dwBytesProcessed == 0) {
                std::cerr << "Client #" << g_clientId << ": ReadFile (read response) failed. GLE=" << GetLastError() << std::endl;
                running = false; break;
            }

            // Process response
            if (message.type == SUCCESS_READ) {
                std::cout << "Client #" << g_clientId << ": Read success." << std::endl;
                DisplayEmployee(message.employee); // [cite: 17]
            }
            else if (message.type == FAIL_READ) {
                std::cout << "Client #" << g_clientId << ": Failed to read record (e.g., not found)." << std::endl;
            }
            else if (message.type == BLOCK_RESPONSE) {
                std::cout << "Client #" << g_clientId << ": Read request blocked by server (record locked)." << std::endl;
            }
            else {
                std::cout << "Client #" << g_clientId << ": Unknown response type: " << message.type << std::endl;
            }
            // "ѕо команде с консоли завершает доступ к записи" [cite: 17] - implied by returning to menu
            break; // Fixed missing break

        case 's': // Modify operation [cite: 13]
            message.type = WRITE_REQUEST;
            std::cout << "Client #" << g_clientId << ": Enter employee ID to modify: ";
            std::cin >> message.employeeId; // [cite: 13]
            while (std::cin.fail()) {
                std::cout << "Invalid input. Please enter a number for employee ID: ";
                std::cin.clear();
                std::cin.ignore(256, '\n');
                std::cin >> message.employeeId;
            }
            std::cin.ignore(256, '\n');

            // Send initial write request to server [cite: 13]
            if (!WriteFile(hNamedPipe, &message, sizeof(Message), &dwBytesProcessed, NULL)) {
                std::cerr << "Client #" << g_clientId << ": WriteFile (write request) failed. GLE=" << GetLastError() << std::endl;
                running = false; break;
            }
            // if (hNeedToReadEvent) SetEvent(hNeedToReadEvent);

            if (WaitForSingleObject(hICanReadEvent, INFINITE) != WAIT_OBJECT_0) {
                std::cerr << "Client #" << g_clientId << ": WaitForSingleObject (hICanReadEvent for write) failed. GLE=" << GetLastError() << std::endl;
                running = false; break;
            }
            // ResetEvent(hICanReadEvent);


            if (!ReadFile(hNamedPipe, &message, sizeof(Message), &dwBytesProcessed, NULL) || dwBytesProcessed == 0) {
                std::cerr << "Client #" << g_clientId << ": ReadFile (write response phase 1) failed. GLE=" << GetLastError() << std::endl;
                running = false; break;
            }

            if (message.type == SUCCESS_READ) { // Server sends current data and confirms lock
                std::cout << "Client #" << g_clientId << ": Record lock acquired. Current data:" << std::endl;
                DisplayEmployee(message.employee); // [cite: 14]
                message.employee = ModifyEmployee(message.employee); // Get new data from user [cite: 14]

                message.type = WRITE_REQUEST_READY; // Change type for sending modified data
                message.clientId = g_clientId; // Ensure client ID is still set

                std::cout << "Client #" << g_clientId << ": Send modified record to server? (y/n): "; // [cite: 15] (simplified)
                char confirm_send;
                std::cin >> confirm_send;
                std::cin.ignore(256, '\n');


                if (confirm_send == 'y' || confirm_send == 'Y') {
                    // Send modified record to server [cite: 15]
                    if (!WriteFile(hNamedPipe, &message, sizeof(Message), &dwBytesProcessed, NULL)) {
                        std::cerr << "Client #" << g_clientId << ": WriteFile (write request ready) failed. GLE=" << GetLastError() << std::endl;
                        running = false; /* TODO: Should also tell server to release lock */ break;
                    }
                    // if (hNeedToReadEvent) SetEvent(hNeedToReadEvent);

                    if (WaitForSingleObject(hICanReadEvent, INFINITE) != WAIT_OBJECT_0) {
                        std::cerr << "Client #" << g_clientId << ": WaitForSingleObject (hICanReadEvent for write phase 2) failed. GLE=" << GetLastError() << std::endl;
                        running = false; break;
                    }
                    // ResetEvent(hICanReadEvent);

                    if (!ReadFile(hNamedPipe, &message, sizeof(Message), &dwBytesProcessed, NULL) || dwBytesProcessed == 0) {
                        std::cerr << "Client #" << g_clientId << ": ReadFile (write response phase 2) failed. GLE=" << GetLastError() << std::endl;
                        running = false; break;
                    }

                    if (message.type == SUCCESS) {
                        std::cout << "Client #" << g_clientId << ": Record modified successfully on server." << std::endl;
                        DisplayEmployee(message.employee); // Display final state from server
                    }
                    else {
                        std::cout << "Client #" << g_clientId << ": Server failed to modify record." << std::endl;
                    }
                }
                else {
                    std::cout << "Client #" << g_clientId << ": Modification cancelled." << std::endl;
                    // TODO: Client should inform server to release the lock without writing.
                    // This requires an additional message type, e.g., CANCEL_WRITE_REQUEST.
                    // For now, the lock on the server will timeout or be cleared when client disconnects.
                }

            }
            else if (message.type == FAIL_READ) {
                std::cout << "Client #" << g_clientId << ": Cannot modify record (e.g., not found)." << std::endl;
            }
            else if (message.type == BLOCK_RESPONSE) {
                std::cout << "Client #" << g_clientId << ": Modify request blocked by server (record locked by another process)." << std::endl;
            }
            else {
                std::cout << "Client #" << g_clientId << ": Unknown response type for modify: " << message.type << std::endl;
            }
            // "ѕо команде с консоли завершает доступ к записи" [cite: 15] - implied by returning to menu
            break;

        default:
            std::cout << "Client #" << g_clientId << ": Invalid choice. Please try again." << std::endl;
            break;
        }
    }

    CleanUpClient();
    std::cout << "Client #" << g_clientId << " exiting." << std::endl;
    return 0;
}