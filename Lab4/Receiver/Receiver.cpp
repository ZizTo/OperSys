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
	SharedData data = { 0, 0, {} };
	onF.write(reinterpret_cast<char*>(&data), sizeof(data));
	onF.close();

	int kolSenders;
	cin >> kolSenders;

	string commLine = "Sender.exe " + binfl;
	vector<STARTUPINFO> siv(kolSenders);
	vector<PROCESS_INFORMATION> piv(kolSenders);

	for (auto si : siv) {
		ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);
	}

	for (int i = 0; i < kolSenders; i++)
	{
		CreateProcess(NULL, strdup(commLine.c_str()), NULL, NULL, FALSE,
			CREATE_NEW_CONSOLE, NULL, NULL, &siv[i], &piv[i]);
	}
	
	while (true) {
		char com;
		cout << "q - quit; r - receive";
		cin >> com;
		if (com == 'q') break;
		else if (com == 'r') {

		}
		else { cout << "Command not found\n"; }
	}

	

	for (auto pi : piv) {
		CloseHandle(pi.hThread);
	}
}