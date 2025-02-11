#include <conio.h>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

struct employee
{
	int num;
	char name[10];
	double hours;
};

int main(int argc, char* argv[]) {
	string name, name2, commLine1, commLine2;
	int kol;
	cout << "File name for creater: ";
	cin >> name;
	cout << "Kol of employees: ";
	cin >> kol;

	commLine1 = "Creator.exe " + name + " " + to_string(kol);

	wstring wcommandLine1(commLine1.begin(), commLine1.end());
	wchar_t* lpszCommandLine = new wchar_t[wcommandLine1.size() + 1];
	wcscpy_s(lpszCommandLine, wcommandLine1.size() + 1, wcommandLine1.c_str());

	STARTUPINFO si;
	PROCESS_INFORMATION piCom;

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);


	CreateProcess(NULL, lpszCommandLine, NULL, NULL, FALSE,
		CREATE_NEW_CONSOLE, NULL, NULL, &si, &piCom);

	WaitForSingleObject(piCom.hThread, INFINITE);
	
	CloseHandle(piCom.hThread);
	CloseHandle(piCom.hProcess);

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
	wstring wcommandLine2(commLine2.begin(), commLine2.end());
	wchar_t* lpszCommandLine2 = new wchar_t[wcommandLine2.size() + 1];
	wcscpy_s(lpszCommandLine2, wcommandLine2.size() + 1, wcommandLine2.c_str());
	
	STARTUPINFO si2;
	PROCESS_INFORMATION piCom2;

	ZeroMemory(&si2, sizeof(STARTUPINFO));
	si2.cb = sizeof(STARTUPINFO);


	CreateProcess(NULL, lpszCommandLine2, NULL, NULL, FALSE,
		CREATE_NEW_CONSOLE, NULL, NULL, &si2, &piCom2);

	WaitForSingleObject(piCom2.hThread, INFINITE);

	CloseHandle(piCom2.hThread);
	CloseHandle(piCom2.hProcess);
	string line;
	ifstream in(name2);
	cout << "Report\n";
	while (getline(in, line))
	{
		cout << line << endl;
	}
	 
	_getch();
	delete[] lpszCommandLine;
	delete[] lpszCommandLine2;

	return 0;
}