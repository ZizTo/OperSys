#include "Reporter.h"

int main(int argc, char* argv[]) {
	if (argc != 4) {
		_cputs("Invalid arguments");
		_getch();
		return 1;
	}
	Reporter* r = new Reporter(argv[1], atoi(argv[3]));
	
	r->getFromFile();
	r->writeOnFile(argv[2]);

	cout << "Report created";
	_getch();
	return 0;
}