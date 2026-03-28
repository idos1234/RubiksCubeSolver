#include "cube.h"
#include <chrono>
#include <thread>

#define SCRAMBLE_MOVES 14

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

	auto start = chrono::steady_clock::now();

	vector<int> steps = c.Solve(SCRAMBLE_MOVES);

	this_thread::sleep_for(std::chrono::milliseconds(150));
	auto end = chrono::steady_clock::now();
	auto duration = chrono::duration_cast<std::chrono::milliseconds>(end - start);

	cout << endl << endl;

	for (int i = 0; i < 20 && steps[i] != 0; ++i)
	{
		switch (steps[i])
		{
		case 1: cout << "U "; break;
		case 2: cout << "U2 "; break;
		case 3: cout << "U' "; break;
		case 4: cout << "F "; break;
		case 5: cout << "F2 "; break;
		case 6: cout << "F' "; break;
		case 7: cout << "R "; break;
		case 8: cout << "R2 "; break;
		case 9: cout << "R' "; break;
		case 10: cout << "B "; break;
		case 11: cout << "B2 "; break;
		case 12: cout << "B' "; break;
		case 13: cout << "L "; break;
		case 14: cout << "L2 "; break;
		case 15: cout << "L' "; break;
		case 16: cout << "D "; break;
		case 17: cout << "D2 "; break;
		case 18: cout << "D' "; break;

		}
	}

	Cube::FreePdb();

	cout << endl << "Task took " << duration.count() / (float)(CLOCKS_PER_SEC) << " seconds to run." << std::endl;
}