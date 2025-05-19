#include "structs.h"
#include <vector>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

string binFl;

vector<STARTUPINFO> siv;
vector<PROCESS_INFORMATION> piv;
HANDLE NeedToRead;
HANDLE hNamedPipe;
HANDLE* YouCanReadNow;
HANDLE* AllDead;

void CleanUp() {
    for (auto pi : piv) {
		CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
	}
	CloseHandle(NeedToRead);
}

void DisplayEmployeeFile() {
    ifstream file(binFl, ios::binary);
    Employee data;
    cout << "\nGuys:\n";
    while (!file.eof()) {
        file.read(reinterpret_cast<char*>(&data), sizeof(data));
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
    while (!file.eof()) {
        file.read(reinterpret_cast<char*>(&data), sizeof(data));
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

}

int main() {
	cout << "Enter file name: ";
	cin >> binFl;
	int count;
    cout << "Enter number of employees: ";
    cin >> count;

	ofstream onF(binFl, ios::binary);

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

    hNamedPipe = CreateNamedPipe(
        "\\\\.\\pipe\\pipe", // имя канала
        PIPE_ACCESS_DUPLEX, // читаем из канала
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES, // максимальное количество экземпляров канала
        0, // размер выходного буфера по умолчанию
        0, // размер входного буфера по умолчанию
        INFINITE, // клиент ждет связь бесконечно долго
        (LPSECURITY_ATTRIBUTES)NULL
    );

    string commLine = "Client98.exe ";
    for (int i = 0; i < ClKol; i++)
    {
        cout << "2";
        CreateProcess(NULL, strdup((commLine + to_string(i)).c_str()), NULL, NULL, FALSE,
			CREATE_NEW_CONSOLE, NULL, NULL, &siv[i], &piv[i]);
        cout << "3";
        if (ConnectNamedPipe(
            hNamedPipe, // дескриптор канала
            (LPOVERLAPPED)NULL // асинхронная связь
        )) {
            cout << "all cool";
        }
    }
    

    bool readOpen = true, modificOpen = true;
    NeedToRead = CreateEvent(NULL, false, false, "ReadEvent");
    HANDLE* AllDead = new HANDLE[ClKol];
    for (int i = 0; i < ClKol; i++)
    {
        AllDead[i] = CreateEvent(NULL, false, false, "Dead" + i);
    }
    YouCanReadNow = new HANDLE[ClKol];
    for (int i = 0; i < ClKol; i++)
    {
        YouCanReadNow[i] = CreateEvent(NULL, false, false, "Read" + i);
    }

    while (true) {
        if (WaitForMultipleObjects(ClKol, AllDead, TRUE, 0) != WAIT_TIMEOUT) {
            break;
        } 

        if (WaitForSingleObject(NeedToRead, 0) != WAIT_TIMEOUT) {
            ResetEvent(NeedToRead);
            DWORD dwBytesRead;
            DWORD dwBytesWrite;
            Message message;
            ReadFile(
                hNamedPipe, // дескриптор канала
                &message, // адрес буфера для ввода данных
                sizeof(Message), // число читаемых байтов
                &dwBytesRead, // число прочитанных байтов
                (LPOVERLAPPED)NULL // передача данных синхронная
            );

            Message answ;

            switch (message.type) {
            case READ_REQUEST:
                if (readOpen) {
                    answ.employee = FindEmployee(message.employeeId);
                    if (answ.employee.num == -1) {
                        answ.type = FAIL_READ;
                    }
                    else {
                        answ.type = SUCCESS_READ;
                    }
                }
                else {
                    answ.type = BLOCK_RESPONSE;
                }
                break;
            case WRITE_REQUEST:
                if (modificOpen) {
                    answ.employee = FindEmployee(message.employeeId);
                    if (answ.employee.num == -1) {
                        answ.type = FAIL_READ;
                    }
                    else {
                        readOpen = false;
                        modificOpen = false;
                        answ.type = SUCCESS_READ;
                    }
                }
                else {
                    answ.type = BLOCK_RESPONSE;
                }
                break;
            case WRITE_REQUEST_READY:
                ChangeEmployee(message.employee.num, message.employee);
                answ.type = SUCCESS;
                message.employee = FindEmployee(message.employeeId);
                readOpen = true;
                modificOpen = true;
                break;

            default:
                cerr << "err";
                return 1;
                break;
            }

            WriteFile(
                hNamedPipe, // дескриптор канала
                &answ, // адрес буфера для вывода данных
                sizeof(Message), // число записываемых байтов
                &dwBytesWrite, // число записанных байтов
                (LPOVERLAPPED)NULL // передача данных синхронная
            );

            SetEvent(YouCanReadNow[message.id]);
        }
    }
    
    char a;
    DisplayEmployeeFile();
    cout << "Enter to close";
    cin >> a;
    CleanUp();
}