#include <gtest/gtest.h>
#include "Lab2.h"

TEST(LabTest, JustTestMin) {
    n = 3;
    mas = std::vector<int>{ 10, 2, -7 };
    HANDLE hThreadMM;
    DWORD IDThreadMM;
    hThreadMM = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)min_max, NULL, 0, &IDThreadMM);
    WaitForSingleObject(hThreadMM, INFINITE);
    CloseHandle(hThreadMM);
    EXPECT_EQ(minEl, 2);
}

TEST(LabTest, JustTestMax) {
    n = 3;
    mas = { 10, 2, -7 };
    HANDLE hThreadMM;
    DWORD IDThreadMM;
    hThreadMM = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)min_max, NULL, 0, &IDThreadMM);
    WaitForSingleObject(hThreadMM, INFINITE);
    CloseHandle(hThreadMM);
    EXPECT_EQ(maxEl, 0);
}

TEST(LabTest, JustTestAvar) {
    n = 3;
    mas = { 10, 2, -6 };
    HANDLE hThreadMM;
    DWORD IDThreadMM;
    hThreadMM = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)avarage, NULL, 0, &IDThreadMM);
    WaitForSingleObject(hThreadMM, INFINITE);
    CloseHandle(hThreadMM);
    EXPECT_EQ(avar, 2);
}

TEST(LabTest, BigTestMin) {
    n = 100;
    mas = { 456, -789, 123, -345, 678, -234, 567, -890, 321, -456, 789, -123, 345, -678, 234, -567, 890, -321, 654, -987, 432, -876, 543, -765, 876, -543, 987, -654, 765, -432, 234, -567, 890, -321, 654, -987, 432, -876, 543, -765, 876, -543, 987, -654, 765, -432, 123, -456, 789, -123, 345, -678, 234, -567, 890, -321, 654, -987, 432, -876, 543, -765, 876, -543, 987, -654, 765, -432, 123, -456, 789, -123, 345, -678, 234, -567, 890, -321, 654, -987, 432, -876, 543, -765, 876, -543, 987, -654, 765, -432, 123, -456, 789, -123, 345, -678, 234, -567, 890, -321 };
    HANDLE hThreadMM;
    DWORD IDThreadMM;
    hThreadMM = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)min_max, NULL, 0, &IDThreadMM);
    WaitForSingleObject(hThreadMM, INFINITE);
    CloseHandle(hThreadMM);
    EXPECT_EQ(mas[minEl], -987);
}

TEST(LabTest, BigTestMax) {
    n = 100;
    mas = { 456, -789, 123, -345, 678, -234, 567, -890, 321, -456, 789, -123, 345, -678, 234, -567, 890, -321, 654, -987, 432, -876, 543, -765, 876, -543, 987, -654, 765, -432, 234, -567, 890, -321, 654, -987, 432, -876, 543, -765, 876, -543, 987, -654, 765, -432, 123, -456, 789, -123, 345, -678, 234, -567, 890, -321, 654, -987, 432, -876, 543, -765, 876, -543, 987, -654, 765, -432, 123, -456, 789, -123, 345, -678, 234, -567, 890, -321, 654, -987, 432, -876, 543, -765, 876, -543, 987, -654, 765, -432, 123, -456, 789, -123, 345, -678, 234, -567, 890, -321 };
    HANDLE hThreadMM;
    DWORD IDThreadMM;
    hThreadMM = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)min_max, NULL, 0, &IDThreadMM);
    WaitForSingleObject(hThreadMM, INFINITE);
    CloseHandle(hThreadMM);
    EXPECT_EQ(mas[maxEl], 987);
}

TEST(LabTest, BigTestAvar) {
    n = 100;
    mas = { 456, -789, 123, -345, 678, -234, 567, -890, 321, -456, 789, -123, 345, -678, 234, -567, 890, -321, 654, -987, 432, -876, 543, -765, 876, -543, 987, -654, 765, -432, 234, -567, 890, -321, 654, -987, 432, -876, 543, -765, 876, -543, 987, -654, 765, -432, 123, -456, 789, -123, 345, -678, 234, -567, 890, -321, 654, -987, 432, -876, 543, -765, 876, -543, 987, -654, 765, -432, 123, -456, 789, -123, 345, -678, 234, -567, 890, -321, 654, -987, 432, -876, 543, -765, 876, -543, 987, -654, 765, -432, 123, -456, 789, -123, 345, -678, 234, -567, 890, -321 };
    HANDLE hThreadMM;
    DWORD IDThreadMM;
    hThreadMM = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)avarage, NULL, 0, &IDThreadMM);
    WaitForSingleObject(hThreadMM, INFINITE);
    CloseHandle(hThreadMM);
    EXPECT_EQ(avar, 9.46);
}