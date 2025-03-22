#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "lab3.h"

int a, suma;
DWORD WINAPI sum(LPVOID lpParam) {
	Sleep(500);
	suma = a + (int)lpParam; 
	return 0;
}

TEST_CASE("Thread start") {
	a = 3; 
	int b = 1;
	HANDLE hThread = CreateThread(NULL, 0, sum, (void*)b, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);
	REQUIRE(suma == (a + b));
	CloseHandle(hThread);
}

TEST_CASE("Thread stop") {
	a = 3;
	int b = 1;
	HANDLE hThread = CreateThread(NULL, 0, sum, (void*)b, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);
	REQUIRE(CloseHandle(hThread));
}

TEST_CASE("Thread marker works") {
	n = 10;
	mas = std::vector<int>(n, -1);

	int thkol;
	thkol = 1;
	thrkol = thkol;

	HANDLE* hThreads = new HANDLE[thkol];
	StopEvent = new HANDLE[thkol];
	ResumeEvent = new HANDLE[thkol];
	DeleteEvent = new HANDLE[thkol];
	IDThreads.resize(thkol);
	isThreadWorks.resize(thkol, true);

	InitializeCriticalSection(&cs);
	InitializeCriticalSection(&csout);

	for (int i = 0; i < thkol; i++)
	{
		StopEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		ResumeEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		DeleteEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		hThreads[i] = CreateThread(NULL, 0, marker, (void*)i, 0, NULL);
	}

	while (true)
	{
		REQUIRE(WaitForMultipleObjects(thkol, StopEvent, TRUE, INFINITE) != WAIT_TIMEOUT);

		int thrId = 0;

		SetEvent(DeleteEvent[thrId]);
		WaitForSingleObject(hThreads[thrId], INFINITE);
		CloseHandle(hThreads[thrId]);

		REQUIRE(thrkol <= 0);
		if (thrkol <= 0) {
			break;
		}
	}

	DeleteCriticalSection(&cs);
	DeleteCriticalSection(&csout);
	for (int i = 0; i < thkol; i++)
	{
		REQUIRE(CloseHandle(StopEvent[i]));
		REQUIRE(CloseHandle(ResumeEvent[i]));
		REQUIRE(CloseHandle(DeleteEvent[i]));
	}
}
