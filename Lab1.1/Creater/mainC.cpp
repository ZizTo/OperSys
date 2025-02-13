#include "Creator.h"


int main(int argc, char* argv[]) {
	_cputs("Hello, i'm creater.\n");

	Creator *cr = new Creator(string(argv[1]));
	int kol = atoi(argv[2]);
	
	for (int i = 0; i < kol; i++) {
		string im;
		double hours;

		cout << "Enter employee with id " + to_string(i + 1) + ": \n";
		cout << "Enter name: ";
		cin >> im;
		cout << "Enter hours: ";
		cin >> hours;
		
		cr->writeEmpl(i + 1, im, hours);
	}

	cout << "All entered";
	_getch();
	return 0;
}