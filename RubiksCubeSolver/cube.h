#include <iostream>
#include <ctime>
#include <algorithm>
#include <stdlib.h>
#include <queue>
#include <vector>


#define CUBESIZE 3
#define PDB_SIZE 88179840

using namespace std;


struct PairFirstComparator {
	bool operator()(const pair<int, char*>& a, const pair<int, char*>& b) const {
		return a.first > b.first;
	}
};

class Cube {
private:
	char cubeMat[6][CUBESIZE][CUBESIZE];
	int numOfTurns;

	vector<const char*> steps;
	static unsigned char* corners_pdb;

public:
	Cube();
	Cube(const char _cubeMat[6][CUBESIZE][CUBESIZE]);

private:
	static void InitPdb();


private:
	void TurnClockWise(char side[CUBESIZE][CUBESIZE]);
	void TurnCounterClockWise(char side[CUBESIZE][CUBESIZE]);
	void Turn180(char side[CUBESIZE][CUBESIZE]);

	void PDB_DFS(int turn = 0);

	bool CornerComp(char a1, char b1, char c1, char a2, char b2, char c2);

public:
	//need to change this to private after testing
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

	void Scramble(int movesCount);

	void Print();

	static void FreePdb();

private:
	bool IsSolved() const;
	bool SolveHelper(int turn = 0);

public:
	vector<const char*> Solve();
};