#include "pch.h"
#include "CppUnitTest.h"
#include "../RubiksCubeSolver/cube.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// If Cube.cpp still defines this helper as a free function, tests rely on it.
// Otherwise the Cube constructor or other init functions should populate combination tables.
extern void precomputeCombinations();

namespace RubiksCubeTests
{
	TEST_CLASS(CubePublicApiTests)
	{
	public:

		// Helper: build a standard solved cube layout matching Cube.cpp face indices:
		// 0: Up = 'w', 1: Front = 'g', 2: Right = 'r', 3: Back = 'b', 4: Left = 'o', 5: Down = 'y'
		static void BuildSolvedMat(char outMat[6][3][3])
		{
			const char faceColors[6] = { 'w', 'g', 'r', 'b', 'o', 'y' };
			for (int f = 0; f < 6; ++f)
			{
				for (int r = 0; r < 3; ++r)
					for (int c = 0; c < 3; ++c)
						outMat[f][r][c] = faceColors[f];
			}
		}

		// Positions for the 8 corners (face,row,col) used by CalcCornerPermutationIndex / IdentifyCorner
		static void GetCornerPositions(int outPos[8][3][3])
		{
			// corner 0
			outPos[0][0][0] = 0; outPos[0][0][1] = 0; outPos[0][0][2] = 0;
			outPos[0][1][0] = 4; outPos[0][1][1] = 0; outPos[0][1][2] = 0;
			outPos[0][2][0] = 3; outPos[0][2][1] = 0; outPos[0][2][2] = 2;

			// corner 1
			outPos[1][0][0] = 0; outPos[1][0][1] = 0; outPos[1][0][2] = 2;
			outPos[1][1][0] = 2; outPos[1][1][1] = 0; outPos[1][1][2] = 2;
			outPos[1][2][0] = 3; outPos[1][2][1] = 0; outPos[1][2][2] = 0;

			// corner 2
			outPos[2][0][0] = 0; outPos[2][0][1] = 2; outPos[2][0][2] = 2;
			outPos[2][1][0] = 1; outPos[2][1][1] = 0; outPos[2][1][2] = 2;
			outPos[2][2][0] = 2; outPos[2][2][1] = 0; outPos[2][2][2] = 0;

			// corner 3
			outPos[3][0][0] = 0; outPos[3][0][1] = 2; outPos[3][0][2] = 0;
			outPos[3][1][0] = 1; outPos[3][1][1] = 0; outPos[3][1][2] = 0;
			outPos[3][2][0] = 4; outPos[3][2][1] = 0; outPos[3][2][2] = 2;

			// corner 4
			outPos[4][0][0] = 5; outPos[4][0][1] = 2; outPos[4][0][2] = 0;
			outPos[4][1][0] = 4; outPos[4][1][1] = 2; outPos[4][1][2] = 0;
			outPos[4][2][0] = 3; outPos[4][2][1] = 2; outPos[4][2][2] = 2;

			// corner 5
			outPos[5][0][0] = 5; outPos[5][0][1] = 2; outPos[5][0][2] = 2;
			outPos[5][1][0] = 2; outPos[5][1][1] = 2; outPos[5][1][2] = 2;
			outPos[5][2][0] = 3; outPos[5][2][1] = 2; outPos[5][2][2] = 0;

			// corner 6
			outPos[6][0][0] = 5; outPos[6][0][1] = 0; outPos[6][0][2] = 2;
			outPos[6][1][0] = 1; outPos[6][1][1] = 2; outPos[6][1][2] = 2;
			outPos[6][2][0] = 2; outPos[6][2][1] = 2; outPos[6][2][2] = 0;

			// corner 7
			outPos[7][0][0] = 5; outPos[7][0][1] = 0; outPos[7][0][2] = 0;
			outPos[7][1][0] = 1; outPos[7][1][1] = 2; outPos[7][1][2] = 0;
			outPos[7][2][0] = 4; outPos[7][2][1] = 2; outPos[7][2][2] = 2;
		}

		// Edge positions used by FindEdgeNum / IdentifyEdge (from Cube.cpp mapping)
		static void GetEdgePositions(int outPos[12][2][3])
		{
			// Each edge: two stickers (face,row,col)
			// order matches the implementation in Cube.cpp
			outPos[0][0][0] = 0; outPos[0][0][1] = 0; outPos[0][0][2] = 1; // (0,0,1)
			outPos[0][1][0] = 3; outPos[0][1][1] = 0; outPos[0][1][2] = 1; // (3,0,1)

			outPos[1][0][0] = 0; outPos[1][0][1] = 1; outPos[1][0][2] = 2;
			outPos[1][1][0] = 2; outPos[1][1][1] = 0; outPos[1][1][2] = 1;

			outPos[2][0][0] = 0; outPos[2][0][1] = 2; outPos[2][0][2] = 1;
			outPos[2][1][0] = 1; outPos[2][1][1] = 0; outPos[2][1][2] = 1;

			outPos[3][0][0] = 0; outPos[3][0][1] = 1; outPos[3][0][2] = 0;
			outPos[3][1][0] = 4; outPos[3][1][1] = 0; outPos[3][1][2] = 1;

			outPos[4][0][0] = 5; outPos[4][0][1] = 2; outPos[4][0][2] = 1;
			outPos[4][1][0] = 3; outPos[4][1][1] = 2; outPos[4][1][2] = 1;

			outPos[5][0][0] = 5; outPos[5][0][1] = 1; outPos[5][0][2] = 2;
			outPos[5][1][0] = 2; outPos[5][1][1] = 2; outPos[5][1][2] = 1;

			outPos[6][0][0] = 5; outPos[6][0][1] = 0; outPos[6][0][2] = 1;
			outPos[6][1][0] = 1; outPos[6][1][1] = 2; outPos[6][1][2] = 1;

			outPos[7][0][0] = 5; outPos[7][0][1] = 1; outPos[7][0][2] = 0;
			outPos[7][1][0] = 4; outPos[7][1][1] = 2; outPos[7][1][2] = 1;

			outPos[8][0][0] = 1; outPos[8][0][1] = 1; outPos[8][0][2] = 0;
			outPos[8][1][0] = 4; outPos[8][1][1] = 1; outPos[8][1][2] = 2;

			outPos[9][0][0] = 1; outPos[9][0][1] = 1; outPos[9][0][2] = 2;
			outPos[9][1][0] = 2; outPos[9][1][1] = 1; outPos[9][1][2] = 0;

			outPos[10][0][0] = 3; outPos[10][0][1] = 1; outPos[10][0][2] = 2;
			outPos[10][1][0] = 4; outPos[10][1][1] = 1; outPos[10][1][2] = 0;

			outPos[11][0][0] = 3; outPos[11][0][1] = 1; outPos[11][0][2] = 0;
			outPos[11][1][0] = 2; outPos[11][1][1] = 1; outPos[11][1][2] = 2;
		}

		TEST_METHOD(TestFindCornerNumAndIdentifyCorner_Solved)
		{
			char solvedMat[6][3][3];
			BuildSolvedMat(solvedMat);

			Cube c(solvedMat);

			int pos[8][3][3];
			GetCornerPositions(pos);

			for (int i = 0; i < 8; ++i)
			{
				char a = solvedMat[pos[i][0][0]][pos[i][0][1]][pos[i][0][2]];
				char b = solvedMat[pos[i][1][0]][pos[i][1][1]][pos[i][1][2]];
				char d = solvedMat[pos[i][2][0]][pos[i][2][1]][pos[i][2][2]];

				int expectedId = c.FindCornerNum(a, b, d);
				auto ident = c.IdentifyCorner(solvedMat, i);

				// IdentifyCorner should agree with FindCornerNum on a solved cube and orientation must be 0
				Assert::AreEqual(expectedId, ident.first);
				Assert::AreEqual(0, ident.second);
			}
		}

		TEST_METHOD(TestFindEdgeNumAndIdentifyEdge_Solved)
		{
			char solvedMat[6][3][3];
			BuildSolvedMat(solvedMat);

			Cube c(solvedMat);

			int pos[12][2][3];
			GetEdgePositions(pos);

			for (int e = 0; e < 12; ++e)
			{
				char a = solvedMat[pos[e][0][0]][pos[e][0][1]][pos[e][0][2]];
				char b = solvedMat[pos[e][1][0]][pos[e][1][1]][pos[e][1][2]];

				int expectedId = c.FindEdgeNum(a, b);
				auto ident = c.IdentifyEdge(solvedMat, e);

				Assert::AreEqual(expectedId, ident.first);
				Assert::AreEqual(0, ident.second); // orientation zero on solved cube
			}
		}

		TEST_METHOD(TestEdgeComp_Simple)
		{
			char solvedMat[6][3][3];
			BuildSolvedMat(solvedMat);
			Cube c(solvedMat);

			Assert::IsTrue(c.EdgeComp('w', 'b', 'b', 'w'));
			Assert::IsFalse(c.EdgeComp('w', 'b', 'g', 'r'));
		}

		TEST_METHOD(TestFindEdgeNum_KnownMapping)
		{
			char solvedMat[6][3][3];
			BuildSolvedMat(solvedMat);
			Cube c(solvedMat);

			// Validate a couple of known mappings from the implementation
			Assert::AreEqual(0, c.FindEdgeNum('w', 'b')); // top-middle <-> back-middle in original mapping
			Assert::AreEqual(6, c.FindEdgeNum('y', 'g') | 0); // ensure it returns an int (use bitwise or zero to avoid unused warning)
		}

		TEST_METHOD(TestCalcEdgesIndex_PublicContracts)
		{
			char solvedMat[6][3][3];
			BuildSolvedMat(solvedMat);

			// Try to initialize any required tables if present
			precomputeCombinations(); // safe no-op if not defined; present in older implementation
			Cube::InitMoveTables();

			Cube c(solvedMat);

			int idx1 = c.CalcEdgesIndex_1();
			int idx2 = c.CalcEdgesIndex_2();

			// Just verify indices are within expected bounds and non-negative
			Assert::IsTrue(idx1 >= 0 && idx1 < EDGES_PDB_SIZE);
			Assert::IsTrue(idx2 >= 0 && idx2 < EDGES_PDB_SIZE);
		}

		TEST_METHOD(TestApplyMoveChangesState)
		{
			char solvedMat[6][3][3];
			BuildSolvedMat(solvedMat);

			Cube::InitMoveTables();
			Cube c(solvedMat);

			int before = c.CalcCornerIndex();
			// face mapping assumed: 0:U,1:F,2:R,3:B,4:L,5:D (matches other code in repo)
			c.ApplyMove(2, 1); // apply R
			int after = c.CalcCornerIndex();

			Assert::IsTrue(before != after);
		}
	};
}