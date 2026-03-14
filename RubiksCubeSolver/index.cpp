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

	c.R();

	c.Print();

	cout << "Done!";
}