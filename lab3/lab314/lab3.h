#pragma once
#include <windows.h>
#include <vector>
#include <iostream>
#include <cmath>
#include <random>
#include <mutex>

std::vector<int> mas;
std::vector<bool> isThreadWorks;
HANDLE* StopEvent;
HANDLE* ResumeEvent;
HANDLE* DeleteEvent;
HANDLE DeadThEvent;
std::mutex cs, csout;
int n = 0;
int thrkol = 0;

DWORD WINAPI marker(int lpParam) {
    srand(lpParam);
    int elmarked = 0;

    while (true) {
        int index = rand() % n;

        cs.lock();
        if (mas[index] == -1) {
            Sleep(5);
            mas[index] = lpParam;
            elmarked++;
            cs.unlock();
            Sleep(5);
        }
        else {
            cs.unlock();

            csout.lock();;
            std::cout << "Thread id" << lpParam << " marked " << elmarked
                << " elements. Stop on " << index << std::endl;
            SetEvent(StopEvent[lpParam]);
            csout.unlock();

            while (true) {
                if (WaitForSingleObject(DeleteEvent[lpParam], 0) != WAIT_TIMEOUT) {
                    isThreadWorks[lpParam] = false;
                    for (int i = 0; i < n; ++i) {
                        if (mas[i] == lpParam) {
                            mas[i] = -1;
                        }
                    }
                    thrkol--;
                    SetEvent(DeadThEvent);
                    return 0;
                }
                else if (WaitForSingleObject(ResumeEvent[lpParam], 0) != WAIT_TIMEOUT) {
                    ResetEvent(ResumeEvent[lpParam]);
                    break;
                }
            }
        }
    }
}