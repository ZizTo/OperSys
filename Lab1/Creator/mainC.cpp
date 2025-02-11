#include <conio.h>
#include <windows.h>
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

struct employee
{
	int num;
	char name[10];
	double hours;
};

int main(int argc, char* argv[]) {
	_cputs("Hello, i'm creater.\n");


	string name(argv[1]);
	int kol = atoi(argv[2]);

	ofstream outF(name, ios::binary);
	
	for (int i = 0; i < kol; i++) {
		cout << "Enter employee with id " + to_string(i + 1) + ": \n";
		employee emp;
		emp.num = i + 1;

		string im;
		cout << "Enter name: ";
		cin >> im;
		strncpy_s(emp.name, im.c_str(), sizeof(emp.name) - 1);

		double hours;
		cout << "Enter hours: ";
		cin >> hours;
		emp.hours = hours;

		outF.write(reinterpret_cast<char*>(&emp), sizeof(emp));
	}
	

	outF.close();
	cout << "All entered";
	_getch();
	return 0;
}