#include <conio.h>
#include <windows.h>
#include <fstream>
#include <string>
#include <iostream>
using namespace std;

struct employee
{
	int num;
	char name[10];
	double hours;
};

int main(int argc, char* argv[]) {
	ifstream inF(argv[1], ios::binary);
	ofstream out(argv[2]);
	while(!inF.eof())
	{
		employee emp;
		inF.read(reinterpret_cast<char*>(&emp), sizeof(emp));
		out << "Employee number " << emp.num << ": " << emp.name << " work for " 
			<< emp.hours << " hours and get " << double(atoi(argv[3])) * emp.hours << endl;
	}
	inF.close();
	
	cout << "Report created";
	_getch();
	return 0;
}