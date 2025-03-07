#include "Lab2.h"
#include <thread>
#include <string>

using namespace std;

int main()
{
	while (n <= 0) {
		cout << "Enter size of array (min array size is 1, max 99999): ";
		string str;
		cin >> str;
		bool cool = true;
		cool = str.size() <= 5;
		if (cool) {
			for (auto a : str) {
				cool = isdigit(a);
				if (!cool) { break; }
			}
		}
		if (cool) { n = stoi(str); }
	}
	cout << "Now enter elements: ";
	for (int i = 0; i < n; i++)
	{
		cout << endl << i+1 << ": ";
		int el;
		cin >> el;
		
		mas.push_back(el);
	}
	thread MMT(min_max);
	thread AVT(avarage);

	MMT.join();
	AVT.join();

	mas[minEl] = round(avar);
	mas[maxEl] = round(avar);

	cout << "\n\nResult: \n";

	for (auto el : mas) {
		cout << el << endl;
	}


	return 0;
}
