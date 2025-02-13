#include <conio.h>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include "structs.h"

using namespace std;

void StartProgramm(string str) {
	STARTUPINFO si;
	PROCESS_INFORMATION piCom;

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);


	CreateProcess(NULL, strdup(str.c_str()), NULL, NULL, FALSE,
		CREATE_NEW_CONSOLE, NULL, NULL, &si, &piCom);

	WaitForSingleObject(piCom.hThread, INFINITE);

	CloseHandle(piCom.hThread);
	CloseHandle(piCom.hProcess);
}

int main(int argc, char* argv[]) {
	string name, name2, commLine1, commLine2;
	int kol;
	cout << "File name for creater: ";
	cin >> name;
	cout << "Kol of employees: ";
	cin >> kol;

	commLine1 = "Creator.exe " + name + " " + to_string(kol);

	StartProgramm(commLine1);

	ifstream inF(name, ios::binary);
	for (int i = 0; i < kol; i++)
	{
		employee emp;
		inF.read(reinterpret_cast<char*>(&emp), sizeof(emp));
		cout << "Employee number " << emp.num << ": " << emp.name << " work for " << emp.hours << " hours\n";
	}
	inF.close();

	cout << "\nEnter file name for reporter: ";
	cin >> name2;
	cout << "Enter dollar in hour: ";
	int sal;
	cin >> sal;

	commLine2 = "Reporter.exe " + name + " " + name2 + " " + to_string(sal);

	StartProgramm(commLine2);

	string line;
	ifstream in(name2);
	cout << "Report\n";
	while (getline(in, line))
	{
		cout << line << endl;
	}

	_getch();

	return 0;
}