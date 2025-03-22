#include "lab3.h"

using namespace std;

int main()
{
	cout << "enter array size: ";
	cin >> n;
	mas = vector<int>(n, -1);

	int thkol;
	cout << "Eneter threads ammount: ";
	cin >> thkol;
	thrkol = thkol;

	thread* hThreads = new thread[thkol];
	StopEvent = new HANDLE[thkol];
	ResumeEvent = new HANDLE[thkol];
	DeleteEvent = new HANDLE[thkol];
	isThreadWorks.resize(thkol, true);
	
	DeadThEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	for (int i = 0; i < thkol; i++)
	{
		StopEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		ResumeEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		DeleteEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		hThreads[i] = thread(marker, i);
	}

	while (true)
	{
		WaitForMultipleObjects(thkol, StopEvent, TRUE, INFINITE);

		cout << "mas before deleting: ";
		for (int i = 0; i < n; i++)
		{
			cout << mas[i] << " ";
		}

		int thrId;
		cout << endl << "Enter id of thread that need to stop: ";
		cin >> thrId;
		while (thrId >= n || thrId < 0 || !isThreadWorks[thrId]) {
			cout << "This thread does not exist, enter antorher: ";
			cin >> thrId;
		}

		ResetEvent(DeadThEvent);
		SetEvent(DeleteEvent[thrId]);
		WaitForSingleObject(DeadThEvent, INFINITE);

		cout << "mas after deleting: ";
		for (int i = 0; i < n; i++)
		{
			cout << mas[i] << " ";
		}
		cout << endl;

		if (thrkol <= 0) {
			cout << "end of cycle";
			break;
		}

		for (int i = 0; i < thkol; i++)
		{
			if (isThreadWorks[i]) {
				ResetEvent(StopEvent[i]);
				SetEvent(ResumeEvent[i]);
			}
			else {
				SetEvent(StopEvent[i]);
			}
		}
	}

	for (int i = 0; i < thkol; i++)
	{
		CloseHandle(StopEvent[i]);
		CloseHandle(ResumeEvent[i]);
		CloseHandle(DeleteEvent[i]);
	}

	return 0;
}
