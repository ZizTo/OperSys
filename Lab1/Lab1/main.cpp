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
	string name, commLine1;
	int kol;
	cin >> name >> kol;

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

	_getch();
	
	delete[] lpszCommandLine;

	return 0;
}