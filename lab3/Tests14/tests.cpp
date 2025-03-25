#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "lab3.h"

HANDLE StopEvent;

int a, suma;
void sum(int lpParam) {
	Sleep(500);
	SetEvent(StopEvent);
	suma = a + lpParam; 
}

TEST_CASE("Thread start") {
	StopEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	a = 3; 
	int b = 1;
	std::thread ath(sum, b);
	ath.join();
	WaitForSingleObject(StopEvent, INFINITE);
	REQUIRE(suma == (a + b));
	REQUIRE(CloseHandle(StopEvent));
}

TEST_CASE("Thread marker works") {
	n = 10;
	mas = std::vector<int>(n, -1);

	int thkol;
	thkol = 1;
	thrkol = thkol;

	thread* hThreads = new thread[thkol];
	StopEvent = new HANDLE[thkol];
	ResumeEvent = new HANDLE[thkol];
	DeleteEvent = new HANDLE[thkol];
	IDThreads.resize(thkol);
	isThreadWorks.resize(thkol, true);

	for (int i = 0; i < thkol; i++)
	{
		StopEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		ResumeEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		DeleteEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		hThreads[i] = thread(marker, i);
	}

	while (true)
	{
		REQUIRE(WaitForMultipleObjects(thkol, StopEvent, TRUE, INFINITE) != WAIT_TIMEOUT);

		int thrId = 0;

		ResetEvent(DeadThEvent);
		SetEvent(DeleteEvent[thrId]);
		WaitForSingleObject(DeadThEvent, INFINITE);

		REQUIRE(thrkol <= 0);
		if (thrkol <= 0) {
			break;
		}
	}

	for (int i = 0; i < thkol; i++)
	{
		REQUIRE(CloseHandle(StopEvent[i]));
		REQUIRE(CloseHandle(ResumeEvent[i]));
		REQUIRE(CloseHandle(DeleteEvent[i]));
	}
}
