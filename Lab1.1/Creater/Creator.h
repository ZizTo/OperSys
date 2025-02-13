#pragma once
#include <conio.h>
#include <windows.h>
#include <fstream>
#include <iostream>
#include <string>
#include "structs.h"

using namespace std;

class Creator {
public:
	Creator(string fn) : fileName(fn) {
		outF = new ofstream(fileName, ios::binary);
	}

	void writeEmpl(int num, string name, double hours) {
		employee emp;
		emp.num = num;
		strncpy_s(emp.name, name.c_str(), sizeof(emp.name) - 1);
		emp.hours = hours;
		outF->write(reinterpret_cast<char*>(&emp), sizeof(emp));
	}

	~Creator() {
		outF->close();
	}
private:
	string fileName;
	ofstream *outF;
};