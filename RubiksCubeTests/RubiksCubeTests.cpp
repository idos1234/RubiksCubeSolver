cpp RubiksCubeTests\RubiksCubeTests.cpp
#include "pch.h"
#include "CppUnitTest.h"
#include "../RubiksCubeSolver/Cube.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace RubiksCubeTests
{
    TEST_CLASS(CornerPermutationTests)
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

        // Positions for the 8 corners as used by Cube::CalcCornerPermutationIndex()
        // Each corner listed as three (face,row,col) entries.
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

        TEST_METHOD(TestFindCornerNum)
        {
            char solvedMat[6][3][3];
            BuildSolvedMat(solvedMat);
            Cube c(solvedMat);

            // Corner 0 expected colors (w, b, o) in any order
            Assert::AreEqual(0, c.FindCornerNum('w', 'b', 'o'));
            Assert::AreEqual(0, c.FindCornerNum('o', 'w', 'b'));
            Assert::AreEqual(0, c.FindCornerNum('b', 'o', 'w'));

            // Corner 6 expected colors (y, g, r) in any order
            Assert::AreEqual(6, c.FindCornerNum('g', 'r', 'y'));
            Assert::AreEqual(6, c.FindCornerNum('r', 'y', 'g'));
        }

        TEST_METHOD(TestInvalidCorner)
        {
            char solvedMat[6][3][3];
            BuildSolvedMat(solvedMat);
            Cube c(solvedMat);

            // Invalid combination: 'w' and 'y' together is not a valid corner
            Assert::AreEqual(-1, c.FindCornerNum('w', 'y', 'b'));
            Assert::AreEqual(-1, c.FindCornerNum('y', 'w', 'g'));
        }

        TEST_METHOD(TestCalcCornerPermutationIndex_Solved)
        {
            char solvedMat[6][3][3];
            BuildSolvedMat(solvedMat);

            Cube c(solvedMat);
            int index = c.CalcCornerPermutationIndex();

            // Solved permutation must map to index 0
            Assert::AreEqual(0, index);
        }

        TEST_METHOD(TestSwapFirstTwoCornersIndex)
        {
            char mat[6][3][3];
            BuildSolvedMat(mat);

            // Swap corner 0 and corner 1 stickers to create permutation {1,0,2,3,4,5,6,7}
            int pos[8][3][3];
            GetCornerPositions(pos);

            // Save corner 0 stickers
            char temp0[3];
            for (int s = 0; s < 3; ++s)
            {
                int f = pos[0][s][0], r = pos[0][s][1], c = pos[0][s][2];
                temp0[s] = mat[f][r][c];
            }

            // Move corner1 -> corner0
            for (int s = 0; s < 3; ++s)
            {
                int f1 = pos[1][s][0], r1 = pos[1][s][1], c1 = pos[1][s][2];
                int f0 = pos[0][s][0], r0 = pos[0][s][1], c0 = pos[0][s][2];
                mat[f0][r0][c0] = mat[f1][r1][c1];
            }

            // Move saved corner0 -> corner1
            for (int s = 0; s < 3; ++s)
            {
                int f1 = pos[1][s][0], r1 = pos[1][s][1], c1 = pos[1][s][2];
                mat[f1][r1][c1] = temp0[s];
            }

            Cube c(mat);
            int result = c.CalcCornerPermutationIndex();

            // For permutation {1,0,2,3,4,5,6,7} expected index = 1 * 7! = 5040
            Assert::AreEqual(5040, result);
        }

        TEST_METHOD(TestNoInvalidCornersInSolved)
        {
            char solvedMat[6][3][3];
            BuildSolvedMat(solvedMat);

            int pos[8][3][3];
            GetCornerPositions(pos);

            Cube c(solvedMat);

            for (int corner = 0; corner < 8; ++corner)
            {
                char a = solvedMat[pos[corner][0][0]][pos[corner][0][1]][pos[corner][0][2]];
                char b = solvedMat[pos[corner][1][0]][pos[corner][1][1]][pos[corner][1][2]];
                char d = solvedMat[pos[corner][2][0]][pos[corner][2][1]][pos[corner][2][2]];

                int id = c.FindCornerNum(a, b, d);
                // None of the corners on a solved cube should be invalid
                Assert::IsTrue(id >= 0 && id < 8);
            }
        }
    };
}