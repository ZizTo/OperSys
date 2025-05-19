#include "structs.h"
#include <vector>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>

using namepsace std;

string binfl;

vector<STARTUPINFO> siv;
vector<PROCESS_INFORMATION> piv;
HANDLE NeedToRead;
vector<HANDLE> hNamedPipev;

void CleanUp() {
    for (auto pi : piv) {
		CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
	}
	CloseHandle(NeedToRead);
	CloseHandle(hNamedPipe);
	CloseHandle(LockedSlot);
}

void DisplayEmployeeFile() {
    ifstream file(binFl, ios::binary);
    Employee data;
    cout << "\nGuys:\n";
    while (!file.eof()) {
        file.read(reinterpret_cast<char*>(&data), sizeof(data));
        cout << "Employee#" << employees[i].num << " - "
            << employees[i].name << " - "
            << employees[i].hours << endl;
    }

    cout << endl;

    file.close();
}


int main() {
	cout << "Enter file name: ";
	cin >> binfl;
	int count;
    cout << "Enter number of employees: ";
    cin >> count;

	ofstream onF(binfl, ios::binary);

    for (int i = 0; i < count; i++)
	{
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

    cout << "Client kol: ";
    int ClKol;
    cin >> ClKol;
    piv.resize(ClKol);
    siv.resize(ClKol);
    for (auto si : siv) {
        ZeroMemory(&si, sizeof(STARTUPINFO));
        si.cb = sizeof(STARTUPINFO);
    }
    string commLine = "Client98.exe";
    for (int i = 0; i < ClKol; i++)
    {
        hNamedPipev[i]=CreateNamedPipe(
            "\\\\.\\pipe\\demo_pipe", // имя канала
            PIPE_ACCESS_DUPLEX, // читаем из канала
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES, // максимальное количество экземпляров канала
            0, // размер выходного буфера по умолчанию
            0, // размер входного буфера по умолчанию
            INFINITE, // клиент ждет связь бесконечно долго
            (LPSECURITY_ATTRIBUTES)NULL
        );
        CreateProcess(NULL, strdup((commLine).c_str()), NULL, NULL, FALSE,
			CREATE_NEW_CONSOLE, NULL, NULL, &siv[i], &piv[i]);
        BOOL ConnectNamedPipe (
            hNamedPipev[i], // дескриптор канала
            (LPOVERLAPPED)None // асинхронная связь
            );
    }

    bool readOpen = true, modificOpen = true;
    NeedToRead = CreateEvent(NULL, false, false, "ReadEvent");
    while (true) {
        if (WaitForMultipleObjects(ClKol, piv.hThread, TRUE, 0) != WAIT_TIMEOUT) {
            break;
        } 

        if (WaitForSingleObject(NeedToRead) != WAIT_TIMEOUT) {
            
        }
    }

    
    
    char a;
    DisplayEmployeeFile();
    cout << "Enter to close";
    cin >> a
}