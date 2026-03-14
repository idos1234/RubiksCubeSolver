#include <iostream>
#include <iomanip>
#include <ctime>
#include <algorithm>


#define CUBESIZE 3

using namespace std;


class Cube {
private:
	char cubeMat[6][CUBESIZE][CUBESIZE];
	int numOfTurns;

	static unsigned char* pdb;

public:
	Cube(const char _cubeMat[6][CUBESIZE][CUBESIZE]);


private:
	void TurnClockWise(char side[CUBESIZE][CUBESIZE]);
	void TurnCounterClockWise(char side[CUBESIZE][CUBESIZE]);
	void Turn180(char side[CUBESIZE][CUBESIZE]);

	void Scramble(int movesCount);

	void PDB_DFS(int turn);

	bool CornerComp(char a1, char b1, char c1, char a2, char b2, char c2);

public:
	int FindCornerNum(char a, char b, char c);
	int CalcCornerPermutationIndex();

public:
	void R();
	void R_P();
	void R2();
	void L();
	void L_P();
	void L2();
	void U();
	void U_P();
	void U2();
	void B();
	void B_P();
	void B2();
	void D();
	void D_P();
	void D2();
	void F();
	void F_P();
	void F2();

	void Print();

};