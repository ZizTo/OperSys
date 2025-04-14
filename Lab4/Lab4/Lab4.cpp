#include <windows.h>
#include "Lab4.h"

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

int main()
{
	cout << "Hello CMake." << endl;
	return 0;
}
