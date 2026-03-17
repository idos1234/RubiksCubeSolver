#include "cube.h"

unsigned char* Cube::corners_pdb = nullptr;
unsigned char* Cube::edges_pdb_1 = nullptr;
unsigned char* Cube::edges_pdb_2 = nullptr;

int C[13][7];

void precomputeCombinations() {
    // reset mat
    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < 7; j++) C[i][j] = 0;
    }

    for (int n = 0; n <= 12; n++) {
        C[n][0] = 1;
        for (int k = 1; k <= n && k <= 6; k++) {
            C[n][k] = C[n - 1][k - 1] + C[n - 1][k];
        }
    }
}

void Cube::InitPdb()
{
    if (Cube::corners_pdb == nullptr)
    {
        Cube::corners_pdb = (unsigned char*)calloc(CORNERS_PDB_SIZE, sizeof(char));
        if (Cube::corners_pdb == nullptr)
        {
            cerr << "Failed to allocate memory for PDB! - corners" << endl;
            exit(EXIT_FAILURE);
        }

        memset(corners_pdb, 0xFF, CORNERS_PDB_SIZE);
    }

    if (Cube::edges_pdb_1 == nullptr)
    {
        Cube::edges_pdb_1 = (unsigned char*)calloc(EDGES_PDB_SIZE, sizeof(char));
        if (Cube::edges_pdb_1 == nullptr)
        {
            cerr << "Failed to allocate memory for PDB! - edges1" << endl;
            exit(EXIT_FAILURE);
        }

        memset(edges_pdb_1, 0xFF, EDGES_PDB_SIZE);
    }

    if (Cube::edges_pdb_2 == nullptr)
    {
        Cube::edges_pdb_2 = (unsigned char*)calloc(EDGES_PDB_SIZE, sizeof(char));
        if (Cube::edges_pdb_2 == nullptr)
        {
            cerr << "Failed to allocate memory for PDB! - edges2" << endl;
            exit(EXIT_FAILURE);
        }

        memset(edges_pdb_2, 0xFF, EDGES_PDB_SIZE);
    }


}

void Cube::FreePdb() {
    if (corners_pdb) {
        free(corners_pdb);
        corners_pdb = nullptr;
    }
}

Cube::Cube(const char _cubeMat[6][CUBESIZE][CUBESIZE]) :numOfTurns(0), steps(20, nullptr) {
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

Cube::Cube() {
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


	*this = Cube(cubeMat);
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
    char temp[CUBESIZE];
    for (int i = 0; i < CUBESIZE; i++) temp[i] = cubeMat[0][i][CUBESIZE - 1];
    for (int i = 0; i < CUBESIZE; i++) {
        cubeMat[0][i][CUBESIZE - 1] = cubeMat[1][i][CUBESIZE - 1];
        cubeMat[1][i][CUBESIZE - 1] = cubeMat[5][i][CUBESIZE - 1];
        cubeMat[5][i][CUBESIZE - 1] = cubeMat[3][CUBESIZE - 1 - i][0];
        cubeMat[3][CUBESIZE - 1 - i][0] = temp[i];
    }
}
void Cube::R_P() {
    TurnCounterClockWise(cubeMat[2]);
    char temp[CUBESIZE];
    for (int i = 0; i < CUBESIZE; i++) temp[i] = cubeMat[0][i][CUBESIZE - 1];
    for (int i = 0; i < CUBESIZE; i++) {
        cubeMat[0][i][CUBESIZE - 1] = cubeMat[3][CUBESIZE - 1 - i][0];
        cubeMat[3][CUBESIZE - 1 - i][0] = cubeMat[5][i][CUBESIZE - 1];
        cubeMat[5][i][CUBESIZE - 1] = cubeMat[1][i][CUBESIZE - 1];
        cubeMat[1][i][CUBESIZE - 1] = temp[i];
    }
}
void Cube::R2() {
    Turn180(cubeMat[2]);
    for (int i = 0; i < CUBESIZE; i++) {
        std::swap(cubeMat[0][i][CUBESIZE - 1], cubeMat[5][i][CUBESIZE - 1]); // Up <-> Down
        std::swap(cubeMat[1][i][CUBESIZE - 1], cubeMat[3][CUBESIZE - 1 - i][0]); // Front <-> Back (flipped)
    }
}

// --- L Moves (Left face - Orange) ---
void Cube::L() {
    TurnClockWise(cubeMat[4]);
    char temp[CUBESIZE];
    for (int i = 0; i < CUBESIZE; i++) temp[i] = cubeMat[0][i][0];
    for (int i = 0; i < CUBESIZE; i++) {
        cubeMat[0][i][0] = cubeMat[3][CUBESIZE - 1 - i][CUBESIZE - 1];
        cubeMat[3][CUBESIZE - 1 - i][CUBESIZE - 1] = cubeMat[5][i][0];
        cubeMat[5][i][0] = cubeMat[1][i][0];
        cubeMat[1][i][0] = temp[i];
    }
}
void Cube::L_P() {
    TurnCounterClockWise(cubeMat[4]);
    char temp[CUBESIZE];
    for (int i = 0; i < CUBESIZE; i++) temp[i] = cubeMat[0][i][0];
    for (int i = 0; i < CUBESIZE; i++) {
        cubeMat[0][i][0] = cubeMat[1][i][0];
        cubeMat[1][i][0] = cubeMat[5][i][0];
        cubeMat[5][i][0] = cubeMat[3][CUBESIZE - 1 - i][CUBESIZE - 1];
        cubeMat[3][CUBESIZE - 1 - i][CUBESIZE - 1] = temp[i];
    }
}
void Cube::L2() {
    Turn180(cubeMat[4]);
    for (int i = 0; i < CUBESIZE; i++) {
        std::swap(cubeMat[0][i][0], cubeMat[5][i][0]); // Up <-> Down
        std::swap(cubeMat[1][i][0], cubeMat[3][CUBESIZE - 1 - i][CUBESIZE - 1]); // Front <-> Back (flipped)
    }
}

// --- U Moves (Up face - White) ---
void Cube::U() {
    TurnClockWise(cubeMat[0]);
    char temp[CUBESIZE];
    for (int i = 0; i < CUBESIZE; i++) temp[i] = cubeMat[1][0][i];
    for (int i = 0; i < CUBESIZE; i++) {
        cubeMat[1][0][i] = cubeMat[2][0][i];
        cubeMat[2][0][i] = cubeMat[3][0][i];
        cubeMat[3][0][i] = cubeMat[4][0][i];
        cubeMat[4][0][i] = temp[i];
    }
}
void Cube::U_P() {
    TurnCounterClockWise(cubeMat[0]);
    char temp[CUBESIZE];
    for (int i = 0; i < CUBESIZE; i++) temp[i] = cubeMat[1][0][i];
    for (int i = 0; i < CUBESIZE; i++) {
        cubeMat[1][0][i] = cubeMat[4][0][i];
        cubeMat[4][0][i] = cubeMat[3][0][i];
        cubeMat[3][0][i] = cubeMat[2][0][i];
        cubeMat[2][0][i] = temp[i];
    }
}
void Cube::U2() {
    Turn180(cubeMat[0]);
    for (int i = 0; i < CUBESIZE; i++) {
        std::swap(cubeMat[1][0][i], cubeMat[3][0][i]); // Front <-> Back
        std::swap(cubeMat[2][0][i], cubeMat[4][0][i]); // Right <-> Left
    }
}

// --- D Moves (Down face - Yellow) ---
void Cube::D() {
    TurnClockWise(cubeMat[5]);
    char temp[CUBESIZE];
    for (int i = 0; i < CUBESIZE; i++) temp[i] = cubeMat[1][CUBESIZE - 1][i];
    for (int i = 0; i < CUBESIZE; i++) {
        cubeMat[1][CUBESIZE - 1][i] = cubeMat[4][CUBESIZE - 1][i];
        cubeMat[4][CUBESIZE - 1][i] = cubeMat[3][CUBESIZE - 1][i];
        cubeMat[3][CUBESIZE - 1][i] = cubeMat[2][CUBESIZE - 1][i];
        cubeMat[2][CUBESIZE - 1][i] = temp[i];
    }
}
void Cube::D_P() {
    TurnCounterClockWise(cubeMat[5]);
    char temp[CUBESIZE];
    for (int i = 0; i < CUBESIZE; i++) temp[i] = cubeMat[1][CUBESIZE - 1][i];
    for (int i = 0; i < CUBESIZE; i++) {
        cubeMat[1][CUBESIZE - 1][i] = cubeMat[2][CUBESIZE - 1][i];
        cubeMat[2][CUBESIZE - 1][i] = cubeMat[3][CUBESIZE - 1][i];
        cubeMat[3][CUBESIZE - 1][i] = cubeMat[4][CUBESIZE - 1][i];
        cubeMat[4][CUBESIZE - 1][i] = temp[i];
    }
}
void Cube::D2() {
    Turn180(cubeMat[5]);
    for (int i = 0; i < CUBESIZE; i++) {
        std::swap(cubeMat[1][CUBESIZE - 1][i], cubeMat[3][CUBESIZE - 1][i]); // Front <-> Back
        std::swap(cubeMat[2][CUBESIZE - 1][i], cubeMat[4][CUBESIZE - 1][i]); // Right <-> Left
    }
}

// --- F Moves (Front face - Green) ---
void Cube::F() {
    TurnClockWise(cubeMat[1]);
    for (int i = 0; i < CUBESIZE; i++) {
        char tempVal = cubeMat[0][CUBESIZE - 1][i];
        cubeMat[0][CUBESIZE - 1][i] = cubeMat[4][CUBESIZE - 1 - i][CUBESIZE - 1];
        cubeMat[4][CUBESIZE - 1 - i][CUBESIZE - 1] = cubeMat[5][0][CUBESIZE - 1 - i];
        cubeMat[5][0][CUBESIZE - 1 - i] = cubeMat[2][i][0];
        cubeMat[2][i][0] = tempVal;
    }
}
void Cube::F_P() {
    TurnCounterClockWise(cubeMat[1]);
    for (int i = 0; i < CUBESIZE; i++) {
        char tempVal = cubeMat[0][CUBESIZE - 1][i];
        cubeMat[0][CUBESIZE - 1][i] = cubeMat[2][i][0];
        cubeMat[2][i][0] = cubeMat[5][0][CUBESIZE - 1 - i];
        cubeMat[5][0][CUBESIZE - 1 - i] = cubeMat[4][CUBESIZE - 1 - i][CUBESIZE - 1];
        cubeMat[4][CUBESIZE - 1 - i][CUBESIZE - 1] = tempVal;
    }
}
void Cube::F2() {
    Turn180(cubeMat[1]);
    for (int i = 0; i < CUBESIZE; i++) {
        std::swap(cubeMat[0][CUBESIZE - 1][i], cubeMat[5][0][CUBESIZE - 1 - i]); // Up (row) <-> Down (row flipped)
        std::swap(cubeMat[2][i][0], cubeMat[4][CUBESIZE - 1 - i][CUBESIZE - 1]); // Right (col) <-> Left (col flipped)
    }
}

// --- B Moves (Back face - Blue) ---
void Cube::B() {
    TurnClockWise(cubeMat[3]);
    for (int i = 0; i < CUBESIZE; i++) {
        char tempVal = cubeMat[0][0][i];
        cubeMat[0][0][i] = cubeMat[2][i][CUBESIZE - 1];
        cubeMat[2][i][CUBESIZE - 1] = cubeMat[5][CUBESIZE - 1][CUBESIZE - 1 - i];
        cubeMat[5][CUBESIZE - 1][CUBESIZE - 1 - i] = cubeMat[4][CUBESIZE - 1 - i][0];
        cubeMat[4][CUBESIZE - 1 - i][0] = tempVal;
    }
}

void Cube::B_P() {
    TurnCounterClockWise(cubeMat[3]);
    for (int i = 0; i < CUBESIZE; i++) {
        char tempVal = cubeMat[0][0][i];
        cubeMat[0][0][i] = cubeMat[4][CUBESIZE - 1 - i][0];
        cubeMat[4][CUBESIZE - 1 - i][0] = cubeMat[5][CUBESIZE - 1][CUBESIZE - 1 - i];
        cubeMat[5][CUBESIZE - 1][CUBESIZE - 1 - i] = cubeMat[2][i][CUBESIZE - 1];
        cubeMat[2][i][CUBESIZE - 1] = tempVal;
    }
}
void Cube::B2() {
    Turn180(cubeMat[3]);
    for (int i = 0; i < CUBESIZE; i++) {
        std::swap(cubeMat[0][0][i], cubeMat[5][CUBESIZE - 1][CUBESIZE - 1 - i]); // Up (row) <-> Down (row flipped)
        std::swap(cubeMat[2][i][CUBESIZE - 1], cubeMat[4][CUBESIZE - 1 - i][0]); // Right (col) <-> Left (col flipped)
    }
}

void Cube::Scramble(int movesCount) {
    srand(static_cast<unsigned int>(time(0))); // Seed for random generation

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

void Cube::PDB_DFS_CORNERS(int turn) {

    if(turn >= 20) {
        return;
	}

    //calculate array index
    int index = this->CalcCornerPermutationIndex();

    if (index < 0 || index >= CORNERS_PDB_SIZE) {
        return;
    }


    //check if state already existed
    if (Cube::corners_pdb[index] != 255) {
        if(Cube::corners_pdb[index] > turn) {
			Cube::corners_pdb[index] = turn;
		}
        return;
    }
    else {
        Cube::corners_pdb[index] = turn;
    }



    //R
    this->R();
    this->PDB_DFS_CORNERS(turn + 1);
    this->R_P();

    this->R_P();
    this->PDB_DFS_CORNERS(turn + 1);
    this->R();

    this->R2();
    this->PDB_DFS_CORNERS(turn + 1);
    this->R2();

    //L
    this->L();
    this->PDB_DFS_CORNERS(turn + 1);
    this->L_P();

    this->L_P();
    this->PDB_DFS_CORNERS(turn + 1);
    this->L();

    this->L2();
    this->PDB_DFS_CORNERS(turn + 1);
    this->L2();

    //U
    this->U();
    this->PDB_DFS_CORNERS(turn + 1);
    this->U_P();

    this->U_P();
    this->PDB_DFS_CORNERS(turn + 1);
    this->U();

    this->U2();
    this->PDB_DFS_CORNERS(turn + 1);
    this->U2();

    //D
    this->D();
    this->PDB_DFS_CORNERS(turn + 1);
    this->D_P();

    this->D_P();
    this->PDB_DFS_CORNERS(turn + 1);
    this->D();

    this->D2();
    this->PDB_DFS_CORNERS(turn + 1);
    this->D2();

    //B
    this->B();
    this->PDB_DFS_CORNERS(turn + 1);
    this->B_P();

    this->B_P();
    this->PDB_DFS_CORNERS(turn + 1);
    this->B();

    this->B2();
    this->PDB_DFS_CORNERS(turn + 1);
    this->B2();

    //F
    this->F();
    this->PDB_DFS_CORNERS(turn + 1);
    this->F_P();

    this->F_P();
    this->PDB_DFS_CORNERS(turn + 1);
    this->F();

    this->F2();
    this->PDB_DFS_CORNERS(turn + 1);
    this->F2();

}

bool Cube::EdgeComp(char a1, char b1, char a2, char b2) {
    //sort colors
    if (a1 > b1) std::swap(a1, b1);
    if (a2 > b2) std::swap(a2, b2);

	return (a1 == a2 && b1 == b2);
}

int Cube::FindEdgeIndex(char a, char b) {
    if(EdgeComp(a, b, cubeMat[0][0][1], cubeMat[3][0][1])) {
        return 0;
    }
    else if(EdgeComp(a, b, cubeMat[0][1][2], cubeMat[2][0][1])) {
        return 1;
    }
    else if(EdgeComp(a, b, cubeMat[0][2][1], cubeMat[1][0][1])) {
        return 2;
    }
    else if(EdgeComp(a, b, cubeMat[0][1][0], cubeMat[4][0][1])) {
        return 3;
    }
    else if(EdgeComp(a, b, cubeMat[5][2][1], cubeMat[3][2][1])) {
        return 4;
    }
    else if(EdgeComp(a, b, cubeMat[5][1][2], cubeMat[2][2][1])) {
        return 5;
    }
    else if(EdgeComp(a, b, cubeMat[5][0][1], cubeMat[1][2][1])) {
        return 6;
    }
    else if(EdgeComp(a, b, cubeMat[5][1][0], cubeMat[4][2][1])) {
        return 7;
    }
    else if(EdgeComp(a, b, cubeMat[1][1][0], cubeMat[4][1][2])) {
        return 8;
    }
    else if(EdgeComp(a, b, cubeMat[1][1][2], cubeMat[2][1][0])) {
        return 9;
    }
    else if(EdgeComp(a, b, cubeMat[3][1][2], cubeMat[4][1][0])) {
        return 10;
    }
    else if(EdgeComp(a, b, cubeMat[3][1][0], cubeMat[2][1][2])) {
        return 11;
	}

    return -1;
}

int Cube::CalcEdgesIndex_1() {
	// calculate combination index of the 6 edges in the first group
    int indexes[6];
	indexes[0] = FindEdgeIndex('w', 'b');
	indexes[1] = FindEdgeIndex('w', 'r');
	indexes[2] = FindEdgeIndex('w', 'g');
	indexes[3] = FindEdgeIndex('w', 'o');
	indexes[4] = FindEdgeIndex('b', 'o');
    indexes[5] = FindEdgeIndex('b', 'r');

	sort(indexes, indexes + 6);
	int combinationIndex = 0;

    for (int i = 0; i < 6; ++i)
    {
		combinationIndex += C[indexes[i]][6-i];
	}

	// calculate permutation index of the 6 edges in the first group
	int permutationIndex = 0;

    for (int i = 0; i < 6; ++i)
    {
        int count = 0;
		int factorial = 1;
        for (int j = i + 1; j < 6; ++j)
        {
            factorial *= 6 - j;
            if (indexes[j] < indexes[i]) {
                ++count;
            }
        }
		permutationIndex += (count * factorial);
    }

    // formula for calculating the final index: combinationIndex * 720 (6!) + permutationIndex
	int indexPos = combinationIndex * 720 + permutationIndex;

	//calculate orientation index of the 6 edges in the first group
	int orientationIndex = 0;
    int factorial = 32;

    for (int i = 0; i < 6; ++i)
    {
		char edgeColor1, edgeColor2;

        switch (indexes[i]) {
        case 0: // edge (w, b)
            edgeColor1 = cubeMat[0][0][1];
            edgeColor2 = cubeMat[3][0][1];
            break;
        case 1: // edge (w, r)
            edgeColor1 = cubeMat[0][1][2];
            edgeColor2 = cubeMat[2][0][1];
            break;
        case 2: // edge (w, g)
            edgeColor1 = cubeMat[0][2][1];
            edgeColor2 = cubeMat[1][0][1];
            break;
        case 3: // edge (w, o)
            edgeColor1 = cubeMat[0][1][0];
            edgeColor2 = cubeMat[4][0][1];
            break;
        case 4: // edge (b, o)
            edgeColor1 = cubeMat[5][2][1];
            edgeColor2 = cubeMat[3][2][1];
            break;
        case 5: // edge (b, r)
            edgeColor1 = cubeMat[5][1][2];
            edgeColor2 = cubeMat[2][2][1];
            break;
        case 6: // edge (g, r)
            edgeColor1 = cubeMat[5][0][1];
            edgeColor2 = cubeMat[1][2][1];
            break;
        case 7: // edge (g, o)
            edgeColor1 = cubeMat[5][1][0];
            edgeColor2 = cubeMat[4][2][1];
            break;
        case 8: // edge (y, b)
            edgeColor1 = cubeMat[1][1][0];
            edgeColor2 = cubeMat[4][1][2];
            break;
        case 9: // edge (y, r)
            edgeColor1 = cubeMat[1][1][2];
            edgeColor2 = cubeMat[2][1][0];
            break;
        case 10: // edge (y, g)
            edgeColor1 = cubeMat[3][1][2];
            edgeColor2 = cubeMat[4][1][0];
            break;
        case 11: // edge (y, o)
            edgeColor1 = cubeMat[3][1][0];
            edgeColor2 = cubeMat[2][1][2];
            break;
        }
        
		// Determine orientation based on the position of the colors
        if (edgeColor1 == 'w' || edgeColor1 == 'y' || edgeColor2 == 'w' || edgeColor2 == 'y') {
            if (edgeColor2 == 'w' || edgeColor2 == 'y') {
				orientationIndex += factorial; // Flip orientation
            }
        }

        else if (edgeColor1 == 'b' || edgeColor1 == 'g' || edgeColor2 == 'b' || edgeColor2 == 'g') {
            if (edgeColor2 == 'b' || edgeColor2 == 'g') {
                orientationIndex += factorial; // Flip orientation
            }
        }

        else if (edgeColor1 == 'r' || edgeColor1 == 'o' || edgeColor2 == 'r' || edgeColor2 == 'o') {
            if (edgeColor2 == 'r' || edgeColor2 == 'o') {
                orientationIndex += factorial; // Flip orientation
            }
        }

		factorial /= 2; // Each edge has 2 possible orientations
    }

	return indexPos * 64 + orientationIndex; // Final index combining position and orientation
}

int Cube::CalcEdgesIndex_2() {
    // calculate combination index of the 6 edges in the second group
    int indexes[6];
    indexes[0] = FindEdgeIndex('g', 'r');
    indexes[1] = FindEdgeIndex('g', 'o');
    indexes[2] = FindEdgeIndex('y', 'b');
    indexes[3] = FindEdgeIndex('y', 'r');
    indexes[4] = FindEdgeIndex('y', 'g');
    indexes[5] = FindEdgeIndex('y', 'o');

    sort(indexes, indexes + 6);
    int combinationIndex = 0;

    for (int i = 0; i < 6; ++i)
    {
        combinationIndex += C[indexes[i]][6 - i];
    }

    // calculate permutation index of the 6 edges in the second group
    int permutationIndex = 0;

    for (int i = 0; i < 6; ++i)
    {
        int count = 0;
        int factorial = 1;
        for (int j = i + 1; j < 6; ++j)
        {
            factorial *= 6 - j;
            if (indexes[j] < indexes[i]) {
                ++count;
            }
        }
        permutationIndex += (count * factorial);
    }

    // formula for calculating the final index: combinationIndex * 720 (6!) + permutationIndex
    int indexPos = combinationIndex * 720 + permutationIndex;

    //calculate orientation index of the 6 edges in the second group
    int orientationIndex = 0;
    int factorial = 32;

    for (int i = 0; i < 6; ++i)
    {
        char edgeColor1, edgeColor2;

        switch (indexes[i]) {
        case 0: // edge (w, b)
            edgeColor1 = cubeMat[0][0][1];
            edgeColor2 = cubeMat[3][0][1];
            break;
        case 1: // edge (w, r)
            edgeColor1 = cubeMat[0][1][2];
            edgeColor2 = cubeMat[2][0][1];
            break;
        case 2: // edge (w, g)
            edgeColor1 = cubeMat[0][2][1];
            edgeColor2 = cubeMat[1][0][1];
            break;
        case 3: // edge (w, o)
            edgeColor1 = cubeMat[0][1][0];
            edgeColor2 = cubeMat[4][0][1];
            break;
        case 4: // edge (b, o)
            edgeColor1 = cubeMat[5][2][1];
            edgeColor2 = cubeMat[3][2][1];
            break;
        case 5: // edge (b, r)
            edgeColor1 = cubeMat[5][1][2];
            edgeColor2 = cubeMat[2][2][1];
            break;
        case 6: // edge (g, r)
            edgeColor1 = cubeMat[5][0][1];
            edgeColor2 = cubeMat[1][2][1];
            break;
        case 7: // edge (g, o)
            edgeColor1 = cubeMat[5][1][0];
            edgeColor2 = cubeMat[4][2][1];
            break;
        case 8: // edge (y, b)
            edgeColor1 = cubeMat[1][1][0];
            edgeColor2 = cubeMat[4][1][2];
            break;
        case 9: // edge (y, r)
            edgeColor1 = cubeMat[1][1][2];
            edgeColor2 = cubeMat[2][1][0];
            break;
        case 10: // edge (y, g)
            edgeColor1 = cubeMat[3][1][2];
            edgeColor2 = cubeMat[4][1][0];
            break;
        case 11: // edge (y, o)
            edgeColor1 = cubeMat[3][1][0];
            edgeColor2 = cubeMat[2][1][2];
            break;
        }

        // Determine orientation based on the position of the colors
        if (edgeColor1 == 'w' || edgeColor1 == 'y' || edgeColor2 == 'w' || edgeColor2 == 'y') {
            if (edgeColor2 == 'w' || edgeColor2 == 'y') {
                orientationIndex += factorial; // Flip orientation
            }
        }

        else if (edgeColor1 == 'b' || edgeColor1 == 'g' || edgeColor2 == 'b' || edgeColor2 == 'g') {
            if (edgeColor2 == 'b' || edgeColor2 == 'g') {
                orientationIndex += factorial; // Flip orientation
            }
        }

        else if (edgeColor1 == 'r' || edgeColor1 == 'o' || edgeColor2 == 'r' || edgeColor2 == 'o') {
            if (edgeColor2 == 'r' || edgeColor2 == 'o') {
                orientationIndex += factorial; // Flip orientation
            }
        }

        factorial /= 2; // Each edge has 2 possible orientations
    }

    return indexPos * 64 + orientationIndex; // Final index combining position and orientation
}

void Cube::PDB_DFS_EDGES_1(int turn) {
    if (turn >= 20) {
        return;
    }

    //calculate array index
    int index = this->CalcEdgesIndex_1();

    if (index < 0 || index >= EDGES_PDB_SIZE) {
        return;
    }

    //check if state already existed
    if (Cube::edges_pdb_1[index] != 255) {
        if (Cube::edges_pdb_1[index] > turn) {
            Cube::edges_pdb_1[index] = turn;
        }
        return;
    }
    else {
        Cube::edges_pdb_1[index] = turn;
    }

    //R
    this->R();
    this->PDB_DFS_EDGES_1(turn + 1);
    this->R_P();

    this->R_P();
    this->PDB_DFS_EDGES_1(turn + 1);
    this->R();

    this->R2();
    this->PDB_DFS_EDGES_1(turn + 1);
    this->R2();

    //L
    this->L();
    this->PDB_DFS_EDGES_1(turn + 1);
    this->L_P();

    this->L_P();
    this->PDB_DFS_EDGES_1(turn + 1);
    this->L();

    this->L2();
    this->PDB_DFS_EDGES_1(turn + 1);
    this->L2();

    //U
    this->U();
    this->PDB_DFS_EDGES_1(turn + 1);
    this->U_P();

    this->U_P();
    this->PDB_DFS_EDGES_1(turn + 1);
    this->U();

    this->U2();
    this->PDB_DFS_EDGES_1(turn + 1);
    this->U2();

    //D
    this->D();
    this->PDB_DFS_EDGES_1(turn + 1);
    this->D_P();

    this->D_P();
    this->PDB_DFS_EDGES_1(turn + 1);
    this->D();

    this->D2();
    this->PDB_DFS_EDGES_1(turn + 1);
    this->D2();

    //B
    this->B();
    this->PDB_DFS_EDGES_1(turn + 1);
    this->B_P();

    this->B_P();
    this->PDB_DFS_EDGES_1(turn + 1);
    this->B();

    this->B2();
    this->PDB_DFS_EDGES_1(turn + 1);
    this->B2();

    //F
    this->F();
    this->PDB_DFS_EDGES_1(turn + 1);
    this->F_P();

    this->F_P();
    this->PDB_DFS_EDGES_1(turn + 1);
    this->F();

    this->F2();
    this->PDB_DFS_EDGES_1(turn + 1);
    this->F2();

}

void Cube::PDB_DFS_EDGES_2(int turn) {
    if (turn >= 20) {
        return;
    }

    //calculate array index
    int index = this->CalcEdgesIndex_2();

    if (index < 0 || index >= EDGES_PDB_SIZE) {
        return;
    }

    //check if state already existed
    if (Cube::edges_pdb_2[index] != 255) {
        if (Cube::edges_pdb_2[index] > turn) {
            Cube::edges_pdb_2[index] = turn;
        }
        return;
    }
    else {
        Cube::edges_pdb_2[index] = turn;
    }

    //R
    this->R();
    this->PDB_DFS_EDGES_2(turn + 1);
    this->R_P();

    this->R_P();
    this->PDB_DFS_EDGES_2(turn + 1);
    this->R();

    this->R2();
    this->PDB_DFS_EDGES_2(turn + 1);
    this->R2();

    //L
    this->L();
    this->PDB_DFS_EDGES_2(turn + 1);
    this->L_P();

    this->L_P();
    this->PDB_DFS_EDGES_2(turn + 1);
    this->L();

    this->L2();
    this->PDB_DFS_EDGES_2(turn + 1);
    this->L2();

    //U
    this->U();
    this->PDB_DFS_EDGES_2(turn + 1);
    this->U_P();

    this->U_P();
    this->PDB_DFS_EDGES_2(turn + 1);
    this->U();

    this->U2();
    this->PDB_DFS_EDGES_2(turn + 1);
    this->U2();

    //D
    this->D();
    this->PDB_DFS_EDGES_2(turn + 1);
    this->D_P();

    this->D_P();
    this->PDB_DFS_EDGES_2(turn + 1);
    this->D();

    this->D2();
    this->PDB_DFS_EDGES_2(turn + 1);
    this->D2();

    //B
    this->B();
    this->PDB_DFS_EDGES_2(turn + 1);
    this->B_P();

    this->B_P();
    this->PDB_DFS_EDGES_2(turn + 1);
    this->B();

    this->B2();
    this->PDB_DFS_EDGES_2(turn + 1);
    this->B2();

    //F
    this->F();
    this->PDB_DFS_EDGES_2(turn + 1);
    this->F_P();

    this->F_P();
    this->PDB_DFS_EDGES_2(turn + 1);
    this->F();

    this->F2();
    this->PDB_DFS_EDGES_2(turn + 1);
    this->F2();

}

bool Cube::IsSolved() const {
	for (int f = 0; f < 6; ++f) {
		char color = this->cubeMat[f][0][0];
		for (int i = 0; i < CUBESIZE; ++i) {
			for (int j = 0; j < CUBESIZE; ++j) {
				if (this->cubeMat[f][i][j] != color) {
					return false;
				}
			}
		}
	}
	return true;
}

bool Cube::SolveHelper(int turn) {
    cout << turn << ": ";
    for (int i = 0; i < turn; ++i) {
        if(steps[i] != nullptr) {
            cout << steps[i] << " ";
		}
	}
	cout << endl;

    if(this->IsSolved()) {
        Cube::steps[turn] = nullptr;
        return true;
	}


    if (turn >= 20) {
        Cube::steps[turn - 1] = nullptr;
        return false;
	}

    priority_queue<pair<int, char*>, vector<pair<int, char*>>, PairFirstComparator> minHeap_States;
    int h1 = 0;
    int index1 = 0;
    int h2 = 0;
    int index2 = 0;
    int h3 = 0;
    int index3 = 0;

    char move[3];


    //check for R
	this->R();

	index1 = this->CalcCornerPermutationIndex();
	h1 = corners_pdb[index1];
	index2 = this->CalcEdgesIndex_1();
	h2 = edges_pdb_1[index2];
	index3 = this->CalcEdgesIndex_2();
	h3 = edges_pdb_2[index3];

	move[0] = 'R';
	move[1] = '\0';
	minHeap_States.push(make_pair(max(max(h1, h2), h3), _strdup(move)));

	this->R_P();

    //check for R_P
    this->R_P();

    index1 = this->CalcCornerPermutationIndex();
    h1 = corners_pdb[index1];
    index2 = this->CalcEdgesIndex_1();
    h2 = edges_pdb_1[index2];
    index3 = this->CalcEdgesIndex_2();
    h3 = edges_pdb_2[index3];

    move[0] = 'R';
    move[1] = '\'';
    move[2] = '\0';
	minHeap_States.push(make_pair(max(max(h1, h2), h3), _strdup(move)));

    this->R();

    //check for R2
    this->R2();

    index1 = this->CalcCornerPermutationIndex();
    h1 = corners_pdb[index1];
    index2 = this->CalcEdgesIndex_1();
    h2 = edges_pdb_1[index2];
    index3 = this->CalcEdgesIndex_2();
    h3 = edges_pdb_2[index3];

	move[0] = 'R';
	move[1] = '2';
	move[2] = '\0';
	minHeap_States.push(make_pair(max(max(h1, h2), h3), _strdup(move)));

    this->R2();

    //check for L
    this->L();

    index1 = this->CalcCornerPermutationIndex();
    h1 = corners_pdb[index1];
    index2 = this->CalcEdgesIndex_1();
    h2 = edges_pdb_1[index2];
    index3 = this->CalcEdgesIndex_2();
    h3 = edges_pdb_2[index3];

	move[0] = 'L';
	move[1] = '\0';
	minHeap_States.push(make_pair(max(max(h1, h2), h3), _strdup(move)));

    this->L_P();

    //check for L_P
    this->L_P();

    index1 = this->CalcCornerPermutationIndex();
    h1 = corners_pdb[index1];
    index2 = this->CalcEdgesIndex_1();
    h2 = edges_pdb_1[index2];
    index3 = this->CalcEdgesIndex_2();
    h3 = edges_pdb_2[index3];

    move[0] = 'L';
    move[1] = '\'';
    move[2] = '\0';
	minHeap_States.push(make_pair(max(max(h1, h2), h3), _strdup(move)));

    this->L();

    //check for L2
    this->L2();

    index1 = this->CalcCornerPermutationIndex();
    h1 = corners_pdb[index1];
    index2 = this->CalcEdgesIndex_1();
    h2 = edges_pdb_1[index2];
    index3 = this->CalcEdgesIndex_2();
    h3 = edges_pdb_2[index3];

	move[0] = 'L';
	move[1] = '2';
	move[2] = '\0';
	minHeap_States.push(make_pair(max(max(h1, h2), h3), _strdup(move)));

    this->L2();

    //check for U
    this->U();

    index1 = this->CalcCornerPermutationIndex();
    h1 = corners_pdb[index1];
    index2 = this->CalcEdgesIndex_1();
    h2 = edges_pdb_1[index2];
    index3 = this->CalcEdgesIndex_2();
    h3 = edges_pdb_2[index3];

	move[0] = 'U';
	move[1] = '\0';
	minHeap_States.push(make_pair(max(max(h1, h2), h3), _strdup(move)));

    this->U_P();

    //check for U_P
    this->U_P();

    index1 = this->CalcCornerPermutationIndex();
    h1 = corners_pdb[index1];
    index2 = this->CalcEdgesIndex_1();
    h2 = edges_pdb_1[index2];
    index3 = this->CalcEdgesIndex_2();
    h3 = edges_pdb_2[index3];

    move[0] = 'U';
    move[1] = '\'';
    move[2] = '\0';
	minHeap_States.push(make_pair(max(max(h1, h2), h3), _strdup(move)));

    this->U();

    //check for U2
    this->U2();

    index1 = this->CalcCornerPermutationIndex();
    h1 = corners_pdb[index1];
    index2 = this->CalcEdgesIndex_1();
    h2 = edges_pdb_1[index2];
    index3 = this->CalcEdgesIndex_2();
    h3 = edges_pdb_2[index3];

	move[0] = 'U';
	move[1] = '2';
	move[2] = '\0';
	minHeap_States.push(make_pair(max(max(h1, h2), h3), _strdup(move)));

    this->U2();

    //check for D
    this->D();

    index1 = this->CalcCornerPermutationIndex();
    h1 = corners_pdb[index1];
    index2 = this->CalcEdgesIndex_1();
    h2 = edges_pdb_1[index2];
    index3 = this->CalcEdgesIndex_2();
    h3 = edges_pdb_2[index3];

	move[0] = 'D';
	move[1] = '\0';
	minHeap_States.push(make_pair(max(max(h1, h2), h3), _strdup(move)));

    this->D_P();

    //check for D_P
    this->D_P();

    index1 = this->CalcCornerPermutationIndex();
    h1 = corners_pdb[index1];
    index2 = this->CalcEdgesIndex_1();
    h2 = edges_pdb_1[index2];
    index3 = this->CalcEdgesIndex_2();
    h3 = edges_pdb_2[index3];

    move[0] = 'D';
    move[1] = '\'';
    move[2] = '\0';
	minHeap_States.push(make_pair(max(max(h1, h2), h3), _strdup(move)));

    this->D();

    //check for D2
    this->D2();

    index1 = this->CalcCornerPermutationIndex();
    h1 = corners_pdb[index1];
    index2 = this->CalcEdgesIndex_1();
    h2 = edges_pdb_1[index2];
    index3 = this->CalcEdgesIndex_2();
    h3 = edges_pdb_2[index3];

	move[0] = 'D';
	move[1] = '2';
	move[2] = '\0';
    minHeap_States.push(make_pair(h1, _strdup(move)));

    this->D2();

    //check for B
    this->B();

    index1 = this->CalcCornerPermutationIndex();
    h1 = corners_pdb[index1];
    index2 = this->CalcEdgesIndex_1();
    h2 = edges_pdb_1[index2];
    index3 = this->CalcEdgesIndex_2();
    h3 = edges_pdb_2[index3];

	move[0] = 'B';
	move[1] = '\0';
	minHeap_States.push(make_pair(max(max(h1, h2), h3), _strdup(move)));

    this->B_P();

    //check for B_P
    this->B_P();

    index1 = this->CalcCornerPermutationIndex();
    h1 = corners_pdb[index1];
    index2 = this->CalcEdgesIndex_1();
    h2 = edges_pdb_1[index2];
    index3 = this->CalcEdgesIndex_2();
    h3 = edges_pdb_2[index3];

    move[0] = 'B';
    move[1] = '\'';
    move[2] = '\0';
	minHeap_States.push(make_pair(max(max(h1, h2), h3), _strdup(move)));

    this->B();

    //check for B2
    this->B2();

    index1 = this->CalcCornerPermutationIndex();
    h1 = corners_pdb[index1];
    index2 = this->CalcEdgesIndex_1();
    h2 = edges_pdb_1[index2];
    index3 = this->CalcEdgesIndex_2();
    h3 = edges_pdb_2[index3];

	move[0] = 'B';
	move[1] = '2';
    move[2] = '\0';
	minHeap_States.push(make_pair(max(max(h1, h2), h3), _strdup(move)));

    this->B2();

    //check for F
    this->F();

    index1 = this->CalcCornerPermutationIndex();
    h1 = corners_pdb[index1];
    index2 = this->CalcEdgesIndex_1();
    h2 = edges_pdb_1[index2];
    index3 = this->CalcEdgesIndex_2();
    h3 = edges_pdb_2[index3];

	move[0] = 'F';
	move[1] = '\0';
	minHeap_States.push(make_pair(max(max(h1, h2), h3), _strdup(move)));

    this->F_P();

    //check for F_P
    this->F_P();

    index1 = this->CalcCornerPermutationIndex();
    h1 = corners_pdb[index1];
    index2 = this->CalcEdgesIndex_1();
    h2 = edges_pdb_1[index2];
    index3 = this->CalcEdgesIndex_2();
    h3 = edges_pdb_2[index3];

    move[0] = 'F';
    move[1] = '\'';
    move[2] = '\0';
	minHeap_States.push(make_pair(max(max(h1, h2), h3), _strdup(move)));

    this->F();

    //check for F2
    this->F2();

    index1 = this->CalcCornerPermutationIndex();
    h1 = corners_pdb[index1];
    index2 = this->CalcEdgesIndex_1();
    h2 = edges_pdb_1[index2];
    index3 = this->CalcEdgesIndex_2();
    h3 = edges_pdb_2[index3];

	move[0] = 'F';
	move[1] = '2';
	move[2] = '\0';
    minHeap_States.push(make_pair(max(max(h1, h2), h3), _strdup(move)));

    this->F2();



    while (!minHeap_States.empty()) {
		char* _move =  minHeap_States.top().second;
		minHeap_States.pop();

        if (strcmp(_move, "R") == 0) this->R();
        else if (strcmp(_move, "R\'") == 0) this->R_P();
        else if (strcmp(_move, "R2") == 0) this->R2();
        else if (strcmp(_move, "L") == 0) this->L();
        else if (strcmp(_move, "L\'") == 0) this->L_P();
        else if (strcmp(_move, "L2") == 0) this->L2();
        else if (strcmp(_move, "U") == 0) this->U();
        else if (strcmp(_move, "U\'") == 0) this->U_P();
        else if (strcmp(_move, "U2") == 0) this->U2();
        else if (strcmp(_move, "D") == 0) this->D();
        else if (strcmp(_move, "D\'") == 0) this->D_P();
        else if (strcmp(_move, "D2") == 0) this->D2();
        else if (strcmp(_move, "B") == 0) this->B();
        else if (strcmp(_move, "B\'") == 0) this->B_P();
        else if (strcmp(_move, "B2") == 0) this->B2();
        else if (strcmp(_move, "F") == 0) this->F();
        else if (strcmp(_move, "F\'") == 0) this->F_P();
        else if (strcmp(_move, "F2") == 0) this->F2();

        steps[turn] = _move;

        if (this->SolveHelper(turn + 1)) {
            return true;
        }
        else {
            // Undo move
            if (strcmp(_move, "R") == 0) this->R_P();
            else if (strcmp(_move, "R\'") == 0) this->R();
            else if (strcmp(_move, "R2") == 0) this->R2();
            else if (strcmp(_move, "L") == 0) this->L_P();
            else if (strcmp(_move, "L\'") == 0) this->L();
            else if (strcmp(_move, "L2") == 0) this->L2();
            else if (strcmp(_move, "U") == 0) this->U_P();
            else if (strcmp(_move, "U\'") == 0) this->U();
            else if (strcmp(_move, "U2") == 0) this->U2();
            else if (strcmp(_move, "D") == 0) this->D_P();
            else if (strcmp(_move, "D\'") == 0) this->D();
            else if (strcmp(_move, "D2") == 0) this->D2();
            else if (strcmp(_move, "B") == 0) this->B_P();
            else if (strcmp(_move, "B\'") == 0) this->B();
            else if (strcmp(_move, "B2") == 0) this->B2();
            else if (strcmp(_move, "F") == 0) this->F_P();
            else if (strcmp(_move, "F\'") == 0) this->F();
            else if (strcmp(_move, "F2") == 0) this->F2();
        }
    }

	return false;
}

vector<const char*> Cube::Solve() {
    precomputeCombinations();

	Cube::InitPdb();
	Cube().PDB_DFS_CORNERS(0);
    Cube().PDB_DFS_EDGES_1(0);
    Cube().PDB_DFS_EDGES_2(0);

    if (SolveHelper(0)) {
        return steps;
    }
    else {
        return {"No solution found"};
    }


}