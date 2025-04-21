#include <windows.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "structs.h"

using namespace std;

int main(int argc, char* argv[]) {
	string binfl;
	int kolZap;
	cout << "Enter file name: ";
	cin >> binfl;
	cout << "Enter number records: ";
	cin >> kolZap;

	ofstream onF(binfl, ios::binary);
	SharedData data = { -1, -1, {'a'}};
	for (int i = 0; i < kolZap; i++)
	{
		onF.write(reinterpret_cast<char*>(&data), sizeof(data));
	}
	onF.close();

	int kolSenders;
	cout << "Enter number of Senders: ";
	cin >> kolSenders;

	string commLine = "Sender98.exe " + binfl;
	vector<STARTUPINFO> siv(kolSenders);
	vector<PROCESS_INFORMATION> piv(kolSenders);

	for (auto si : siv) {
		ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);
	}
	HANDLE* eventReady = new HANDLE[kolSenders];
	HANDLE mutexFile = CreateMutex(NULL, false, "mutexFile");
	HANDLE FreeSlot = CreateSemaphore(NULL, kolZap, kolZap, "FreeSlot");
	HANDLE LockedSlot = CreateSemaphore(NULL, 0, kolZap, "LockedSlot");

	for (int i = 0; i < kolSenders; i++)
	{
		eventReady[i] = CreateEvent(NULL, false, false, "ReadyN"+i);
		CreateProcess(NULL, strdup((commLine + " " + to_string(i)).c_str()), NULL, NULL, FALSE,
			CREATE_NEW_CONSOLE, NULL, NULL, &siv[i], &piv[i]);
	}

	WaitForMultipleObjects(kolSenders, eventReady, TRUE, INFINITE);
	
	int messageID = -1;
	while (true) {
		string com;
		cout << "q - quit; r - receive\nEnter command: ";
		cin >> com;
		if (com == "q") break;
		else if (com == "r") {
			WaitForSingleObject(LockedSlot, INFINITE);
			WaitForSingleObject(mutexFile, INFINITE);

			ifstream file(binfl, ios::binary);

			messageID++;
			while(!file.eof()) {
				file.read(reinterpret_cast<char*>(&data), sizeof(data));
				if (data.messageID == messageID) break;
			}
			cout << "Thread " << data.writerID << " say: " << data.message << endl;
			file.close();

			ReleaseMutex(mutexFile);
			ReleaseSemaphore(FreeSlot, 1, NULL);
		}
		else { cout << "Command not found\n"; }
	}

	

	for (auto pi : piv) {
		CloseHandle(pi.hThread);
	}
	for (int i = 0; i < kolSenders; i++) {
		CloseHandle(eventReady[i]);
	}
	CloseHandle(mutexFile);
	CloseHandle(FreeSlot);
	CloseHandle(LockedSlot);
}