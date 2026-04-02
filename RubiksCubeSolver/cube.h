#include <iostream>
#include <ctime>
#include <algorithm>
#include <stdlib.h>
#include <queue>
#include <vector>

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

	static int CP_MoveTable[40320][18];
	static int CO_MoveTable[2187][18];

	static int (*Edges1_MoveTable)[18];
	static int (*Edges2_MoveTable)[18];

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

	void ResetCubeEdges();

private:
	static void LoadPdb();
	static void CreatePdb();
	static void LoadEdgesMoveTables();

public:
	static void InitMoveTables();
	static void GenerateCPMoveTable();
	static void GenerateCOMoveTable();
	static void GenerateEdges1MoveTable();
	static void GenerateEdges2MoveTable();


private:
	void ManualApplyMove(int face);
	void ApplyMove(int face, int times);


	//dfs for building the PDB for corners
	void PDB_BFS_CORNERS();
	int CalcCornerIndex();

	int CalcCornerOrientationIndex();
	void DecodeCornerOrientation(int index);

	int CalcCornerPermutationIndex();
	void DecodeCornerPermutation(int index);

	int FindCornerNum(char c1, char c2, char c3);
	bool CornerComp(char a1, char b1, char c1, char a2, char b2, char c2);
	pair<int, int> IdentifyCorner(const char cubeMat[6][CUBESIZE][CUBESIZE], int position);


	pair<int, int> IdentifyEdge(const char cubeMat[6][CUBESIZE][CUBESIZE], int position);
	bool EdgeComp(char a1, char b1, char a2, char b2);
	int FindEdgeNum(char a, char b);
	int CalcEdgesIndex_1();
	void DecodeEdges_1(int fullIndex);

	int CalcEdgesIndex_2();
	void DecodeEdges_2(int fullIndex);

	int CalcEdgeOrientationIndex_1();
	void DecodeEdgeOrientation_1(int index, int* activePositions);

	int CalcEdgePermutationIndex_1();
	void DecodeEdgePermutation_1(int index, int* activePositions);

	int CalcEdgePositionIndex_1();
	void DecodeEdgePosition_1(int index, int* activePositions);

	int CalcEdgeOrientationIndex_2();
	void DecodeEdgeOrientation_2(int index, int* activePositions);

	int CalcEdgePermutationIndex_2();
	void DecodeEdgePermutation_2(int index, int* activePositions);

	int CalcEdgePositionIndex_2();
	void DecodeEdgePosition_2(int index, int* activePositions);

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
	int GetHeuristic(int cpIdx, int coIdx, int e1Idx, int e2Idx);
	bool ShouldSkipMove(int currentFace, int lastFace);
	bool SolveHelper(int turn, int limit, int lastFace, int cpIdx, int coIdx, int e1Idx, int e2Idx);

public:
	vector<int> Solve(int limit);
};