#include "cube.h"

int main() {
	char cubeMat[6][CUBESIZE][CUBESIZE] = {
		{
			{'w','w','w'},
			{'w','w','w'},
			{'w','w','w'}
		},
		{
			{'g','g','g'},
			{'g','g','g'},
			{'g','g','g'}
		},
		{
			{'r','r','r'},
			{'r','r','r'},
			{'r','r','r'}
		},
		{
			{'b','b','b'},
			{'b','b','b'},
			{'b','b','b'}
		},
		{
			{'o','o','o'},
			{'o','o','o'},
			{'o','o','o'}
		},
		{
			{'y','y','y'},
			{'y','y','y'},
			{'y','y','y'}
		},
	};

	Cube c(cubeMat);

	c.Scramble(3);

	c.Print();

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