#include "cube.h"


unsigned char* Cube::pdb = nullptr;

Cube::Cube(const char _cubeMat[6][CUBESIZE][CUBESIZE]) :numOfTurns(0) {
	for (int i = 0; i < 6; ++i)
	{
		for (int j = 0; j < CUBESIZE; ++j)
		{
			for (int k = 0; k < CUBESIZE; ++k)
			{
				this->cubeMat[i][j][k] = _cubeMat[i][j][k];
			}
		}
	}
}


void Cube::TurnClockWise(char side[CUBESIZE][CUBESIZE]) {
    char temp[CUBESIZE][CUBESIZE] = {};
    // Copy to temp matrix with clockwise mapping: [i][j] -> [j][last-i]
    for (int i = 0; i < CUBESIZE; i++) {
        for (int j = 0; j < CUBESIZE; j++) {
            temp[j][CUBESIZE - 1 - i] = side[i][j];
        }
    }
    // Copy back to original face
    for (int i = 0; i < CUBESIZE; i++) {
        for (int j = 0; j < CUBESIZE; j++) {
            side[i][j] = temp[i][j];
        }
    }
}

void Cube::TurnCounterClockWise(char side[CUBESIZE][CUBESIZE]) {
    char temp[CUBESIZE][CUBESIZE] = {};
    // Copy to temp matrix with counter-clockwise mapping: [i][j] -> [last-j][i]
    for (int i = 0; i < CUBESIZE; i++) {
        for (int j = 0; j < CUBESIZE; j++) {
            temp[CUBESIZE - 1 - j][i] = side[i][j];
        }
    }
    // Copy back to original face
    for (int i = 0; i < CUBESIZE; i++) {
        for (int j = 0; j < CUBESIZE; j++) {
            side[i][j] = temp[i][j];
        }
    }
}

void Cube::Turn180(char side[CUBESIZE][CUBESIZE]) {
    // A 180-degree turn simply swaps elements across the center
    // [0,0] swaps with [2,2], [0,1] with [2,1], etc.
    for (int i = 0; i < (CUBESIZE * CUBESIZE) / 2; i++) {
        swap(side[i / CUBESIZE][i % CUBESIZE],
            side[CUBESIZE - 1 - i / CUBESIZE][CUBESIZE - 1 - i % CUBESIZE]);
    }
}

// --- R Moves (Right face - Red) ---
void Cube::R() {
    TurnClockWise(cubeMat[2]);
    char temp[CUBESIZE] = {};
    for (int i = 0; i < CUBESIZE; i++) temp[i] = cubeMat[0][i][2]; // Store Up edge

    for (int i = 0; i < CUBESIZE; i++) cubeMat[0][i][2] = cubeMat[1][i][2]; // Up <- Front
    for (int i = 0; i < CUBESIZE; i++) cubeMat[1][i][2] = cubeMat[5][i][2]; // Front <- Down
    for (int i = 0; i < CUBESIZE; i++) cubeMat[5][i][2] = cubeMat[3][CUBESIZE - 1 - i][0]; // Down <- Back (flip)
    for (int i = 0; i < CUBESIZE; i++) cubeMat[3][CUBESIZE - 1 - i][0] = temp[i]; // Back <- Up (flip)
}

void Cube::R_P() {
    TurnCounterClockWise(cubeMat[2]);
    char temp[CUBESIZE] = {};
    for (int i = 0; i < CUBESIZE; i++) temp[i] = cubeMat[0][i][2]; // Store Up edge

    for (int i = 0; i < CUBESIZE; i++) cubeMat[0][i][2] = cubeMat[3][CUBESIZE - 1 - i][0]; // Up <- Back (flip)
    for (int i = 0; i < CUBESIZE; i++) cubeMat[3][CUBESIZE - 1 - i][0] = cubeMat[5][i][2]; // Back <- Down (flip)
    for (int i = 0; i < CUBESIZE; i++) cubeMat[5][i][2] = cubeMat[1][i][2]; // Down <- Front
    for (int i = 0; i < CUBESIZE; i++) cubeMat[1][i][2] = temp[i]; // Front <- Up
}

void Cube::R2() {
    Turn180(cubeMat[2]);
    for (int i = 0; i < CUBESIZE; i++) {
        swap(cubeMat[0][i][2], cubeMat[5][i][2]); // Up <-> Down
        swap(cubeMat[1][i][2], cubeMat[3][CUBESIZE - 1 - i][0]); // Front <-> Back
    }
}

// --- L Moves (Left face - Orange) ---
void Cube::L() {
    TurnClockWise(cubeMat[4]);
    char temp[CUBESIZE] = {};
    for (int i = 0; i < CUBESIZE; i++) temp[i] = cubeMat[0][i][0]; // Store Up edge

    for (int i = 0; i < CUBESIZE; i++) cubeMat[0][i][0] = cubeMat[3][CUBESIZE - 1 - i][2]; // Up <- Back (flip)
    for (int i = 0; i < CUBESIZE; i++) cubeMat[3][CUBESIZE - 1 - i][2] = cubeMat[5][i][0]; // Back <- Down (flip)
    for (int i = 0; i < CUBESIZE; i++) cubeMat[5][i][0] = cubeMat[1][i][0]; // Down <- Front
    for (int i = 0; i < CUBESIZE; i++) cubeMat[1][i][0] = temp[i]; // Front <- Up
}

void Cube::L_P() {
    TurnCounterClockWise(cubeMat[4]);
    char temp[CUBESIZE] = {};
    for (int i = 0; i < CUBESIZE; i++) temp[i] = cubeMat[0][i][0]; // Store Up edge

    for (int i = 0; i < CUBESIZE; i++) cubeMat[0][i][0] = cubeMat[1][i][0]; // Up <- Front
    for (int i = 0; i < CUBESIZE; i++) cubeMat[1][i][0] = cubeMat[5][i][0]; // Front <- Down
    for (int i = 0; i < CUBESIZE; i++) cubeMat[5][i][0] = cubeMat[3][CUBESIZE - 1 - i][2]; // Down <- Back (flip)
    for (int i = 0; i < CUBESIZE; i++) cubeMat[3][CUBESIZE - 1 - i][2] = temp[i]; // Back <- Up (flip)
}

void Cube::L2() {
    Turn180(cubeMat[4]);
    for (int i = 0; i < CUBESIZE; i++) {
        swap(cubeMat[0][i][0], cubeMat[5][i][0]); // Up <-> Down
        swap(cubeMat[1][i][0], cubeMat[3][CUBESIZE - 1 - i][2]); // Front <-> Back
    }
}

// --- U Moves (Up face - White) ---
void Cube::U() {
    TurnClockWise(cubeMat[0]);
    char temp[CUBESIZE] = {};
    for (int i = 0; i < CUBESIZE; i++) temp[i] = cubeMat[1][0][i]; // Store Front top row

    for (int i = 0; i < CUBESIZE; i++) cubeMat[1][0][i] = cubeMat[2][0][i]; // Front <- Right
    for (int i = 0; i < CUBESIZE; i++) cubeMat[2][0][i] = cubeMat[3][0][i]; // Right <- Back
    for (int i = 0; i < CUBESIZE; i++) cubeMat[3][0][i] = cubeMat[4][0][i]; // Back <- Left
    for (int i = 0; i < CUBESIZE; i++) cubeMat[4][0][i] = temp[i]; // Left <- Front
}

void Cube::U_P() {
    TurnCounterClockWise(cubeMat[0]);
    char temp[CUBESIZE] = {};
    for (int i = 0; i < CUBESIZE; i++) temp[i] = cubeMat[1][0][i]; // Store Front top row

    for (int i = 0; i < CUBESIZE; i++) cubeMat[1][0][i] = cubeMat[4][0][i]; // Front <- Left
    for (int i = 0; i < CUBESIZE; i++) cubeMat[4][0][i] = cubeMat[3][0][i]; // Left <- Back
    for (int i = 0; i < CUBESIZE; i++) cubeMat[3][0][i] = cubeMat[2][0][i]; // Back <- Right
    for (int i = 0; i < CUBESIZE; i++) cubeMat[2][0][i] = temp[i]; // Right <- Front
}

void Cube::U2() {
    Turn180(cubeMat[0]);
    for (int i = 0; i < CUBESIZE; i++) {
        swap(cubeMat[1][0][i], cubeMat[3][0][i]); // Front <-> Back
        swap(cubeMat[2][0][i], cubeMat[4][0][i]); // Right <-> Left
    }
}

// --- D Moves (Down face - Yellow) ---
void Cube::D() {
    TurnClockWise(cubeMat[5]);
    char temp[CUBESIZE] = {};
    for (int i = 0; i < CUBESIZE; i++) temp[i] = cubeMat[1][2][i]; // Store Front bottom row

    for (int i = 0; i < CUBESIZE; i++) cubeMat[1][2][i] = cubeMat[4][2][i]; // Front <- Left
    for (int i = 0; i < CUBESIZE; i++) cubeMat[4][2][i] = cubeMat[3][2][i]; // Left <- Back
    for (int i = 0; i < CUBESIZE; i++) cubeMat[3][2][i] = cubeMat[2][2][i]; // Back <- Right
    for (int i = 0; i < CUBESIZE; i++) cubeMat[2][2][i] = temp[i]; // Right <- Front
}

void Cube::D_P() {
    TurnCounterClockWise(cubeMat[5]);
    char temp[CUBESIZE] = {};
    for (int i = 0; i < CUBESIZE; i++) temp[i] = cubeMat[1][2][i]; // Store Front bottom row

    for (int i = 0; i < CUBESIZE; i++) cubeMat[1][2][i] = cubeMat[2][2][i]; // Front <- Right
    for (int i = 0; i < CUBESIZE; i++) cubeMat[2][2][i] = cubeMat[3][2][i]; // Right <- Back
    for (int i = 0; i < CUBESIZE; i++) cubeMat[3][2][i] = cubeMat[4][2][i]; // Back <- Left
    for (int i = 0; i < CUBESIZE; i++) cubeMat[4][2][i] = temp[i]; // Left <- Front
}

void Cube::D2() {
    Turn180(cubeMat[5]);
    for (int i = 0; i < CUBESIZE; i++) {
        swap(cubeMat[1][2][i], cubeMat[3][2][i]); // Front <-> Back
        swap(cubeMat[2][2][i], cubeMat[4][2][i]); // Right <-> Left
    }
}

// --- F Moves (Front face - Green) ---
void Cube::F() {
    TurnClockWise(cubeMat[1]);
    char temp[CUBESIZE] = {};
    for (int i = 0; i < CUBESIZE; i++) temp[i] = cubeMat[0][2][i]; // Store Up bottom row

    for (int i = 0; i < CUBESIZE; i++) cubeMat[0][2][i] = cubeMat[4][CUBESIZE - 1 - i][2]; // Up <- Left column
    for (int i = 0; i < CUBESIZE; i++) cubeMat[4][i][2] = cubeMat[5][0][i]; // Left <- Down top row
    for (int i = 0; i < CUBESIZE; i++) cubeMat[5][0][i] = cubeMat[2][CUBESIZE - 1 - i][0]; // Down <- Right column
    for (int i = 0; i < CUBESIZE; i++) cubeMat[2][i][0] = temp[i]; // Right <- Up
}

void Cube::F_P() {
    TurnCounterClockWise(cubeMat[1]);
    char temp[CUBESIZE] = {};
    for (int i = 0; i < CUBESIZE; i++) temp[i] = cubeMat[0][2][i]; // Store Up bottom row

    for (int i = 0; i < CUBESIZE; i++) cubeMat[0][2][i] = cubeMat[2][i][0]; // Up <- Right column
    for (int i = 0; i < CUBESIZE; i++) cubeMat[2][i][0] = cubeMat[5][0][CUBESIZE - 1 - i]; // Right <- Down top row (flip)
    for (int i = 0; i < CUBESIZE; i++) cubeMat[5][0][i] = cubeMat[4][i][2]; // Down <- Left column
    for (int i = 0; i < CUBESIZE; i++) cubeMat[4][CUBESIZE - 1 - i][2] = temp[i]; // Left <- Up (flip)
}

void Cube::F2() {
    Turn180(cubeMat[1]);
    for (int i = 0; i < CUBESIZE; i++) {
        swap(cubeMat[0][2][i], cubeMat[5][0][CUBESIZE - 1 - i]); // Up <-> Down
        swap(cubeMat[2][i][0], cubeMat[4][CUBESIZE - 1 - i][2]); // Right <-> Left
    }
}

// --- B Moves (Back face - Blue) ---
void Cube::B() {
    TurnClockWise(cubeMat[3]);
    char temp[CUBESIZE] = {};
    for (int i = 0; i < CUBESIZE; i++) temp[i] = cubeMat[0][0][i]; // Store Up top row

    for (int i = 0; i < CUBESIZE; i++) cubeMat[0][0][i] = cubeMat[2][i][2]; // Up <- Right column
    for (int i = 0; i < CUBESIZE; i++) cubeMat[2][i][2] = cubeMat[5][2][CUBESIZE - 1 - i]; // Right <- Down bottom row
    for (int i = 0; i < CUBESIZE; i++) cubeMat[5][2][i] = cubeMat[4][i][0]; // Down <- Left column
    for (int i = 0; i < CUBESIZE; i++) cubeMat[4][i][0] = temp[CUBESIZE - 1 - i]; // Left <- Up top row
}

void Cube::B_P() {
    TurnCounterClockWise(cubeMat[3]);
    char temp[CUBESIZE] = {};
    for (int i = 0; i < CUBESIZE; i++) temp[i] = cubeMat[0][0][i]; // Store Up top row

    for (int i = 0; i < CUBESIZE; i++) cubeMat[0][0][i] = cubeMat[4][CUBESIZE - 1 - i][0]; // Up <- Left column
    for (int i = 0; i < CUBESIZE; i++) cubeMat[4][i][0] = cubeMat[5][2][i]; // Left <- Down bottom row
    for (int i = 0; i < CUBESIZE; i++) cubeMat[5][2][i] = cubeMat[2][CUBESIZE - 1 - i][2]; // Down <- Right column
    for (int i = 0; i < CUBESIZE; i++) cubeMat[2][i][2] = temp[i]; // Right <- Up
}

void Cube::B2() {
    Turn180(cubeMat[3]);
    for (int i = 0; i < CUBESIZE; i++) {
        swap(cubeMat[0][0][i], cubeMat[5][2][CUBESIZE - 1 - i]); // Up <-> Down
        swap(cubeMat[2][i][2], cubeMat[4][CUBESIZE - 1 - i][0]); // Right <-> Left
    }
}

void Cube::Scramble(int movesCount) {
    srand(time(0)); // Seed for random generation

    int lastFace = -1; // To avoid moving the same face twice in a row

    for (int i = 0; i < movesCount; i++) {
        int face;
        // Ensure we don't pick a move on the same face as the previous move
        do {
            face = rand() % 6; // 0:U, 1:F, 2:R, 3:B, 4:L, 5:D
        } while (face == lastFace);

        int type = rand() % 3; // 0: Clockwise, 1: Counter-Clockwise, 2: Double (2)

        // Execute move directly based on face and type
        switch (face) {
        case 0: // Up (White)
            if (type == 0) U(); else if (type == 1) U_P(); else U2();
            break;
        case 1: // Front (Green)
            if (type == 0) F(); else if (type == 1) F_P(); else F2();
            break;
        case 2: // Right (Red)
            if (type == 0) R(); else if (type == 1) R_P(); else R2();
            break;
        case 3: // Back (Blue)
            if (type == 0) B(); else if (type == 1) B_P(); else B2();
            break;
        case 4: // Left (Orange)
            if (type == 0) L(); else if (type == 1) L_P(); else L2();
            break;
        case 5: // Down (Yellow)
            if (type == 0) D(); else if (type == 1) D_P(); else D2();
            break;
        }

        lastFace = face;
    }
}

void Cube::Print() {
    auto printFaceRow = [&](int face, int row) {
        for (int col = 0; col < CUBESIZE; col++) {
            cout << cubeMat[face][row][col] << " ";
        }
        };

    // Print Up
    for (int i = 0; i < CUBESIZE; i++) {
        cout << "          "; // Padding for Up face
        printFaceRow(0, i);
        cout << endl;
    }
    cout << endl;

    // Print Left, Front, Right, Back row
    for (int i = 0; i < CUBESIZE; i++) {
        printFaceRow(4, i); cout << "  "; // Left
        printFaceRow(1, i); cout << "  "; // Front
        printFaceRow(2, i); cout << "  "; // Right
        printFaceRow(3, i); cout << "  "; // Back
        cout << endl;
    }
    cout << endl;

    // Print Down
    for (int i = 0; i < CUBESIZE; i++) {
        cout << "          ";
        printFaceRow(5, i);
        cout << endl;
    }
    cout << "-----------------------" << endl;
}

bool Cube::CornerComp(char a1, char b1, char c1, char a2, char b2, char c2) {
    //sort colors
    if (a1 > b1) std::swap(a1, b1);
    if (b1 > c1) std::swap(b1, c1);
    if (a1 > b1) std::swap(a1, b1);

    if (a2 > b2) std::swap(a2, b2);
    if (b2 > c2) std::swap(b2, c2);
    if (a2 > b2) std::swap(a2, b2);

    return (a1 == a2 && b1 == b2 && c1 == c2);
}

int Cube::FindCornerNum(char a, char b, char c) {
    if (this->CornerComp(a, b, c, 'w', 'b', 'o')) {
        return 0;
    }
    else if (this->CornerComp(a, b, c, 'w', 'b', 'r')) {
        return 1;
    }
    else if (this->CornerComp(a, b, c, 'w', 'g', 'r')) {
        return 2;
    }
    else if (this->CornerComp(a, b, c, 'w', 'g', 'o')) {
        return 3;
    }
    else if (this->CornerComp(a, b, c, 'y', 'b', 'o')) {
        return 4;
    }
    else if (this->CornerComp(a, b, c, 'y', 'b', 'r')) {
        return 5;
    }
    else if (this->CornerComp(a, b, c, 'y', 'g', 'r')) {
        return 6;
    }
    else if (this->CornerComp(a, b, c, 'y', 'g', 'o')) {
        return 7;
    }

    return -1;
}

int Cube::CalcCornerPermutationIndex() {
    int indexes[8] = {};

    indexes[0] = FindCornerNum(this->cubeMat[0][0][0], this->cubeMat[4][0][0], this->cubeMat[3][0][2]);
    indexes[1] = FindCornerNum(this->cubeMat[0][0][2], this->cubeMat[2][0][2], this->cubeMat[3][0][0]);
    indexes[2] = FindCornerNum(this->cubeMat[0][2][2], this->cubeMat[1][0][2], this->cubeMat[2][0][0]);
    indexes[3] = FindCornerNum(this->cubeMat[0][2][0], this->cubeMat[1][0][0], this->cubeMat[4][0][2]);
    indexes[4] = FindCornerNum(this->cubeMat[5][2][0], this->cubeMat[4][2][0], this->cubeMat[3][2][2]);
    indexes[5] = FindCornerNum(this->cubeMat[5][2][2], this->cubeMat[2][2][2], this->cubeMat[3][2][0]);
    indexes[6] = FindCornerNum(this->cubeMat[5][0][2], this->cubeMat[1][2][2], this->cubeMat[2][2][0]);
    indexes[7] = FindCornerNum(this->cubeMat[5][0][0], this->cubeMat[1][2][0], this->cubeMat[4][2][2]);


    int index = 0;
    for (int i = 0; i < 8; ++i)
    {
        int count = 0;
        int factorial = 1;

        for (int j = i + 1; j < 8; ++j)
        {
            factorial *= 8 - j;
            if (indexes[j] < indexes[i]) {
                ++count;
            }
        }

        index += (count * factorial);

    }

    return index;

}

void Cube::PDB_DFS(int turn) {
    //calculate array index
    int index = this->CalcCornerPermutationIndex();


    //check if state already existed
    if (this->pdb[index] != 0) {
        return;
    }



    //R
    this->R();
    this->PDB_DFS(turn + 1);
    this->R_P();

    this->R_P();
    this->PDB_DFS(turn + 1);
    this->R();

    this->R2();
    this->PDB_DFS(turn + 1);
    this->R2();

    //L
    this->L();
    this->PDB_DFS(turn + 1);
    this->L_P();

    this->L_P();
    this->PDB_DFS(turn + 1);
    this->L();

    this->L2();
    this->PDB_DFS(turn + 1);
    this->L2();

    //U
    this->U();
    this->PDB_DFS(turn + 1);
    this->U_P();

    this->U_P();
    this->PDB_DFS(turn + 1);
    this->U();

    this->U2();
    this->PDB_DFS(turn + 1);
    this->U2();

    //D
    this->D();
    this->PDB_DFS(turn + 1);
    this->D_P();

    this->D_P();
    this->PDB_DFS(turn + 1);
    this->D();

    this->D2();
    this->PDB_DFS(turn + 1);
    this->D2();

    //B
    this->B();
    this->PDB_DFS(turn + 1);
    this->B_P();

    this->B_P();
    this->PDB_DFS(turn + 1);
    this->B();

    this->B2();
    this->PDB_DFS(turn + 1);
    this->B2();

    //F
    this->F();
    this->PDB_DFS(turn + 1);
    this->F_P();

    this->F_P();
    this->PDB_DFS(turn + 1);
    this->F();

    this->F2();
    this->PDB_DFS(turn + 1);
    this->F2();

}