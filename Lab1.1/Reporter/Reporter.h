#pragma once
#include <conio.h>
#include <windows.h>
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include "structs.h"
using namespace std;

class Reporter
{
public:
	Reporter(string, double);
	~Reporter();
	void writeOnFile(string);
	void getFromFile();
	vector<string> getLines();
private:
	vector<string> lines;
	ifstream* inF;
	double sal;
};

Reporter::Reporter(string name, double s)
{
	inF = new ifstream(name, ios::binary);
	sal = s;
}

Reporter::~Reporter()
{
	inF->close();
}

inline void Reporter::writeOnFile(string name)
{
	ofstream out(name);
	for (auto line : lines) {
		out << line << endl;
	}
		
	out.close();
}

inline void Reporter::getFromFile()
{
	
	while (!inF->eof())
	{
		employee emp;
		inF->read(reinterpret_cast<char*>(&emp), sizeof(emp));
		string strh = to_string(emp.hours);
		string strs = to_string(double(sal) * emp.hours);
		strh = strh.substr(0, strh.find_last_not_of("0.") + 1);
		strs = strs.substr(0, strs.find_last_not_of("0.") + 1);
		if (strh.size() == 0) { strh = "0"; }
		if (strs.size() == 0) { strs = "0"; }
		if (!inF->eof()) {
			lines.push_back("Employee number " + to_string(emp.num) + ": " + emp.name + 
				" work for " + strh + " hours and get " + strs);
		}
	}
}

inline vector<string> Reporter::getLines()
{
	return lines;
}
