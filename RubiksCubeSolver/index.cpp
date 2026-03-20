#include "cube.h"

int main() {
	Cube::InitMoveTables();


	char cubeMat[6][CUBESIZE][CUBESIZE] = {
		{
			{'W','W','W'},
			{'W','W','W'},
			{'W','W','W'}
		},
		{
			{'G','G','G'},
			{'G','G','G'},
			{'G','G','G'}
		},
		{
			{'R','R','R'},
			{'R','R','R'},
			{'R','R','R'}
		},
		{
			{'B','B','B'},
			{'B','B','B'},
			{'B','B','B'}
		},
		{
			{'O','O','O'},
			{'O','O','O'},
			{'O','O','O'}
		},
		{
			{'Y','Y','Y'},
			{'Y','Y','Y'},
			{'Y','Y','Y'}
		},
	};


	Cube c(cubeMat);

	c.R();

	cout << endl << endl << "Solving..." << endl << endl;

	vector<const char*> steps = c.Solve();

	cout << endl << endl;

	for (int i = 0; i < 20 && steps[i] != nullptr; ++i)
	{
		cout << steps[i] << ", ";
	}

	Cube::FreePdb();

	cout << "Done!";
}