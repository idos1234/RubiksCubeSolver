#include "cube.h"

#define SCRAMBLE_MOVES 12

int main() {
	Cube::InitMoveTables();
	precomputeCombinations();


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

	Cube c;
	c.Scramble(SCRAMBLE_MOVES);

	cout << endl << endl << "Solving..." << endl << endl;

	vector<int> steps = c.Solve(SCRAMBLE_MOVES);

	cout << endl << endl;

	for (int i = 0; i < 20 && steps[i] != 0; ++i)
	{
		switch (steps[i])
		{
		case 1: cout << "R "; break;
		case 2: cout << "R' "; break;
		case 3: cout << "R2 "; break;
		case 4: cout << "L "; break;
		case 5: cout << "L' "; break;
		case 6: cout << "L2 "; break;
		case 7: cout << "U "; break;
		case 8: cout << "U' "; break;
		case 9: cout << "U2 "; break;
		case 10: cout << "D "; break;
		case 11: cout << "D' "; break;
		case 12: cout << "D2 "; break;
		case 13: cout << "F "; break;
		case 14: cout << "F' "; break;
		case 15: cout << "F2 "; break;
		case 16: cout << "B "; break;
		case 17: cout << "B' "; break;
		case 18: cout << "B2 "; break;
		}
	}

	Cube::FreePdb();

	cout << "Done!";
}