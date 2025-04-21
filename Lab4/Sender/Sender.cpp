#include <windows.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "structs.h"

using namespace std;

int main(int argc, char* argv[]) { 
	string binFl = argv[1];
	int threadId = atoi(argv[2]);

	HANDLE mutexFile = OpenMutex(MUTEX_ALL_ACCESS, FALSE, "mutexFile");
	HANDLE FreeSlot = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "FreeSlot");
	HANDLE LockedSlot = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "LockedSlot");
	HANDLE letsgoEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, "ReadyN" + threadId);

	SetEvent(letsgoEvent);

	string com;
	SharedData data;
	while (true){
		cout << "Enter command (q - quit; s - send): ";
		cin >> com;
		if (com == "q") break;
		else if (com == "s") {
			string messageStr;
			cout << "Enter message: ";
			cin >> messageStr;
			WaitForSingleObject(FreeSlot, INFINITE);
			WaitForSingleObject(mutexFile, INFINITE);

			ifstream file(binFl, ios::binary);
			vector<SharedData> datamas;

			int lowestID = INT_MAX;
			int maxID = -1;
			while (!file.eof()) {
				file.read(reinterpret_cast<char*>(&data), sizeof(data));
				if (lowestID > data.messageID) { lowestID = data.messageID; }
				if (maxID < data.messageID) { maxID = data.messageID; }
				datamas.push_back(data);
				cout << data.messageID << ": from " << data.writerID << " - " << data.message << endl;
			}
			datamas.erase(datamas.begin() + datamas.size() - 1);
			file.close();

			for (int i = 0; i < datamas.size(); i++)
			{
				if (datamas[i].messageID == lowestID) {
					strncpy_s(datamas[i].message, messageStr.c_str(), sizeof(datamas[i].message) - 1);
					datamas[i].messageID = maxID + 1;
					datamas[i].writerID = threadId;
					break;
				}
			}
			/*for (int i = 0; i < datamas.size(); i++)
			{
				if (datamas[i].messageID == lowestID) {
					datamas.erase(datamas.begin() + i);
					break;
				}
			}*/
			
			std::ofstream ofs;
			ofs.open(binFl, ios::binary | ofstream::out | ofstream::trunc);
			ofs.close();

			ofstream out(binFl, ios::binary);
			for (auto eldata : datamas) {
				out.write(reinterpret_cast<char*>(&eldata), sizeof(eldata));
				cout << "------------ WHAT WE GET ------------\n";
				cout << eldata.messageID << ": from " << eldata.writerID << " - " << eldata.message << endl;
			}

			out.close();
			ReleaseMutex(mutexFile);
			ReleaseSemaphore(LockedSlot, 1, NULL);
		}
		else { cout << "command not found\n"; }
	}

	CloseHandle(mutexFile);
	CloseHandle(FreeSlot);
	CloseHandle(LockedSlot);
	CloseHandle(letsgoEvent);
}