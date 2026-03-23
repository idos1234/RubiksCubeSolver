#include <iostream>
#include <ctime>
#include <algorithm>
#include <stdlib.h>
#include <queue>
#include <vector>

#include "PDBReader.cpp"


#define CUBESIZE 3
#define CORNERS_PDB_SIZE 88179840
#define EDGES_PDB_SIZE 42577920

using namespace std;

void precomputeCombinations();

struct PairFirstComparator {
	bool operator()(const pair<int, int>& a, const pair<int, int>& b) const {
		return a.first > b.first;
	}
};

class Cube {
private:
	// Corners
	signed char cp[8]; // Corner Permutation
	signed char co[8]; // Corner Orientation

	//Edges
	signed char ep[12]; // Edge Permutation
	signed char eo[12]; // Edge Orientation

	// Move tables for corners and edges
	static signed char CP_PERM[18][8];
	static signed char CO_ORI[18][8];
	static signed char EP_PERM[18][12];
	static signed char EO_ORI[18][12];

	int numOfTurns;

	vector<int> steps;
	static unsigned char* corners_pdb;
	static unsigned char* edges_pdb_1;
	static unsigned char* edges_pdb_2;

	// precomputed combinations for calculating PDB indices
	static const int facts_8[8];
	static const int facts_6[6];


public:
	Cube();
	Cube(const char _cubeMat[6][CUBESIZE][CUBESIZE]);

private:
	static void InitPdb();

public:
	static void InitMoveTables();


private:
	void ManualApplyMove(int face);
	void ApplyMove(int face, int times);


	//dfs for building the PDB for corners
	void PDB_BFS_CORNERS();
	void UpdateTurnForChildren_Corners(int turn, int index);
	int CalcCornerIndex();
	int FindCornerNum(char c1, char c2, char c3);
	bool CornerComp(char a1, char b1, char c1, char a2, char b2, char c2);
	pair<int, int> IdentifyCorner(const char cubeMat[6][CUBESIZE][CUBESIZE], int position);


	pair<int, int> IdentifyEdge(const char cubeMat[6][CUBESIZE][CUBESIZE], int position);
	bool EdgeComp(char a1, char b1, char a2, char b2);
	int FindEdgeNum(char a, char b);
	int CalcEdgesIndex_1();
	int CalcEdgesIndex_2();
	void UpdateTurnForChildren_Edges1(int turn, int index);
	void UpdateTurnForChildren_Edges2(int turn, int index);
	void PDB_BFS_EDGES_1();
	void PDB_BFS_EDGES_2();


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

	static void FreePdb();

private:
	int GetHeuristic();
	bool IsSolved() const;
	bool SolveHelper(int turn = 0, int limit = 20);

public:
	vector<int> Solve(int limit);
};