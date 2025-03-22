#pragma once
#include <windows.h>
#include <vector>
#include <iostream>
#include <cmath>
#include <random>

std::vector<int> mas;
std::vector<DWORD> IDThreads;
std::vector<bool> isThreadWorks;
HANDLE *StopEvent;
HANDLE *ResumeEvent;
HANDLE *DeleteEvent;
CRITICAL_SECTION cs, csout;
int n = 0;
int thrkol = 0;

DWORD WINAPI marker(LPVOID lpParam) {
    srand((int)lpParam);
    int elmarked = 0;

    while (true) {
        int index = rand() % n;

        EnterCriticalSection(&cs);
        if (mas[index] == -1) {
            Sleep(5);
            mas[index] = (int)lpParam;
            elmarked++;
            LeaveCriticalSection(&cs);
            Sleep(5);
        }
        else {
            LeaveCriticalSection(&cs);

            EnterCriticalSection(&csout);
            std::cout << "Thread id" << (int)lpParam << " marked " << elmarked
                << " elements. Stop on " << index << std::endl;
            SetEvent(StopEvent[(int)lpParam]);
            LeaveCriticalSection(&csout);

            while (true) {
                if (WaitForSingleObject(DeleteEvent[(int)lpParam], 0) != WAIT_TIMEOUT) {
                    isThreadWorks[(int)lpParam] = false;
                    for (int i = 0; i < n; ++i) {
                        if (mas[i] == (int)lpParam) {
                            mas[i] = -1;
                        }
                    }
                    thrkol--;
                    return 0;
                }
                else if (WaitForSingleObject(ResumeEvent[(int)lpParam], 0) != WAIT_TIMEOUT) {
                    ResetEvent(ResumeEvent[(int)lpParam]);
                    break;
                }
            }
        }
    }
}