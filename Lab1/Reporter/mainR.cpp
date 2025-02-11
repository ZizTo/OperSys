#include <conio.h>
#include <windows.h>
#include <fstream>
#include <string>
using namespace std;

int main(int argc, char* argv[]) {
	_cputs("Hello, i'm creater.\n");
	string name(argv[0]);
	int kol = atoi(argv[1]);
	_getch();
	return 0;
}