#pragma once

#include <windows.h>
#include <vector>
#include <iostream>
#include <cmath>

std::vector<int> mas;
int minEl = 0, maxEl = 0, n;
double avar = 0;

void min_max() {
	for (int i = 1; i < n; i++)
	{
		if (mas[minEl] > mas[i]) { minEl = i; }
		Sleep(7);
		if (mas[maxEl] < mas[i]) { maxEl = i; }
		Sleep(7);
	}
	std::cout << "Min-max thread ended, min element - " << mas[minEl] << ", max element - " << mas[maxEl] << "\n";
}

void avarage() {
	for (int el : mas) {
		avar += el;
		Sleep(12);
	}
	avar /= mas.size();

	std::cout << "Avarage thread ended, avarage - " << avar << "\n";
}
