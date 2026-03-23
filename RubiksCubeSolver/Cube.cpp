#include "cube.h"

unsigned char* Cube::corners_pdb = nullptr;
unsigned char* Cube::edges_pdb_1 = nullptr;
unsigned char* Cube::edges_pdb_2 = nullptr;

signed char Cube::EP_PERM[18][12];
signed char Cube::EO_ORI[18][12];
signed char Cube::CP_PERM[18][8];
signed char Cube::CO_ORI[18][8];

const int Cube::facts_8[8] = { 5040, 720, 120, 24, 6, 2, 1, 1 };
const int Cube::facts_6[6] = { 120, 24, 6, 2, 1, 1 };

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
	// allocate memory for PDBs - used for bulding PDBs

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

	Cube solvedCube;
    Cube::corners_pdb[0] = 0;
	Cube::edges_pdb_1[0] = 0;
    Cube::edges_pdb_2[solvedCube.CalcEdgesIndex_2()] = 0;



}

void Cube::ManualApplyMove(int face) {
    signed char old_cp[8], old_co[8];
    signed char old_ep[12], old_eo[12];
    for (int i = 0; i < 8; i++) { old_cp[i] = cp[i]; old_co[i] = co[i]; }
    for (int i = 0; i < 12; i++) { old_ep[i] = ep[i]; old_eo[i] = eo[i]; }

	// helper lambdas to set corners and edges based on the old state and the move's effect
    auto setC = [&](int dest, int src, int ori) {
        cp[dest] = old_cp[src];
        co[dest] = (old_co[src] + ori) % 3;
        };

    auto setE = [&](int dest, int src, bool flip) {
        ep[dest] = old_ep[src];
        eo[dest] = flip ? (1 - old_eo[src]) : old_eo[src];
        };

    switch (face) {
    case 0: // U (Up)
        setC(0, 1, 0); setC(1, 2, 0); setC(2, 3, 0); setC(3, 0, 0);
        setE(0, 1, false); setE(1, 2, false); setE(2, 3, false); setE(3, 0, false);
        break;

    case 1: // F (Front)
        setC(2, 1, 1); setC(1, 0, 2); setC(0, 5, 1); setC(5, 2, 2);
        setE(3, 7, true); setE(7, 11, true); setE(11, 8, true); setE(8, 3, true);
        break;

    case 2: // R (Right)
        setC(1, 6, 1); setC(6, 7, 2); setC(7, 2, 1); setC(2, 1, 2);
        setE(2, 6, false); setE(6, 10, false); setE(10, 1, false); setE(1, 2, false);
        break;

    case 3: // B (Back)
        setC(0, 3, 1); setC(3, 2, 2); setC(2, 7, 1); setC(7, 0, 2);
        setE(1, 5, true); setE(5, 9, true); setE(9, 6, true); setE(6, 1, true);
        break;

    case 4: // L (Left)
        setC(0, 3, 1); setC(3, 4, 2); setC(4, 5, 1); setC(5, 0, 2);
        setE(8, 11, false); setE(11, 4, false); setE(4, 9, false); setE(9, 8, false);
        break;

    case 5: // D (Down)
        setC(4, 5, 0); setC(5, 6, 0); setC(6, 7, 0); setC(7, 4, 0);
        setE(4, 5, false); setE(5, 6, false); setE(6, 7, false); setE(7, 4, false);
        break;
    }
}
void Cube::InitMoveTables() {
    for (int f = 0; f < 6; f++) {
        for (int t = 1; t <= 3; t++) {
            int moveIdx = f * 3 + (t - 1);

            Cube temp;
            
            for (int i = 0; i < t; i++) {
                temp.ManualApplyMove(f);
            }

            // copy results to move tables
            for (int i = 0; i < 8; i++) {
                CP_PERM[moveIdx][i] = temp.cp[i];
                CO_ORI[moveIdx][i] = temp.co[i];
            }
            for (int i = 0; i < 12; i++) {
                EP_PERM[moveIdx][i] = temp.ep[i];
                EO_ORI[moveIdx][i] = temp.eo[i];
            }
        }
    }
}

void Cube::FreePdb() {
    if (corners_pdb) {
        free(corners_pdb);
        corners_pdb = nullptr;
    }
}

Cube::Cube(const char cubeMat[6][CUBESIZE][CUBESIZE]) :numOfTurns(0), steps(20, 0) {
	// Identify corners and edges from the cubeMat
    for (int i = 0; i < 8; i++) {
        auto result = IdentifyCorner(cubeMat, i);
        cp[i] = result.first;
        co[i] = result.second;
    }

	// Identify edges and their orientations
    for (int i = 0; i < 12; i++) {
        auto result = IdentifyEdge(cubeMat, i);
        ep[i] = result.first;
        eo[i] = result.second;
    }
}

Cube::Cube() :numOfTurns(0), steps(20, 0) {
	// each corner in its place, orientation 0
    for (int i = 0; i < 8; i++) {
        cp[i] = i;
        co[i] = 0;
    }
	// each edge in its place, orientation 0
    for (int i = 0; i < 12; i++) {
        ep[i] = i;
        eo[i] = 0;
    }
}

void Cube::ApplyMove(int face, int times) {
    int moveIdx = face * 3 + (times - 1);

    signed char next_cp[8], next_co[8];
    signed char next_ep[12], next_eo[12];

	// update corners - only position and orientation of corners!
    for (int i = 0; i < 8; i++) {
        int src = CP_PERM[moveIdx][i];
        next_cp[i] = cp[src];
        next_co[i] = (co[src] + CO_ORI[moveIdx][i] + 3) % 3;
    }

	// update edges - only position and orientation of edges!
    for (int i = 0; i < 12; i++) {
        int src = EP_PERM[moveIdx][i];
        next_ep[i] = ep[src];
        next_eo[i] = (eo[src] + EO_ORI[moveIdx][i] + 2) % 2;
    }

	// copy back the results to the cube's state
    for (int i = 0; i < 8; i++) {
        cp[i] = next_cp[i];
        co[i] = next_co[i];
    }
    for (int i = 0; i < 12; i++) {
        ep[i] = next_ep[i];
        eo[i] = next_eo[i];
    }
}

// --- R Moves (Right face - Red) ---
void Cube::R() {
	this->ApplyMove(2, 1);
}
void Cube::R_P() {
    this->ApplyMove(2, 3);
}
void Cube::R2() {
    this->ApplyMove(2, 2);
}

// --- L Moves (Left face - Orange) ---
void Cube::L() {
    this->ApplyMove(4, 1);
}
void Cube::L_P() {
    this->ApplyMove(4, 3);
}
void Cube::L2() {
    this->ApplyMove(4, 2);
}

// --- U Moves (Up face - White) ---
void Cube::U() {
    this->ApplyMove(0, 1);
}
void Cube::U_P() {
    this->ApplyMove(0, 3);
}
void Cube::U2() {
    this->ApplyMove(0, 2);
}

// --- D Moves (Down face - Yellow) ---
void Cube::D() {
    this->ApplyMove(5, 1);
}
void Cube::D_P() {
    this->ApplyMove(5, 3);
}
void Cube::D2() {
    this->ApplyMove(5, 2);
}

// --- F Moves (Front face - Green) ---
void Cube::F() {
    this->ApplyMove(1, 1);
}
void Cube::F_P() {
    this->ApplyMove(1, 3);
}
void Cube::F2() {
    this->ApplyMove(1, 2);
}

// --- B Moves (Back face - Blue) ---
void Cube::B() {
    this->ApplyMove(3, 1);
}
void Cube::B_P() {
    this->ApplyMove(3, 3);
}
void Cube::B2() {
    this->ApplyMove(3, 2);
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

// corners
pair<int, int> Cube::IdentifyCorner(const char mat[6][CUBESIZE][CUBESIZE], int position) {
    char c1, c2, c3;

    // identify the colors of the corner pieces based on their position in the cube
    switch (position) {
    case 0: c1 = mat[0][0][0]; c2 = mat[4][0][0]; c3 = mat[3][0][2]; break; // ULB
    case 1: c1 = mat[0][0][2]; c2 = mat[2][0][2]; c3 = mat[3][0][0]; break; // URB
    case 2: c1 = mat[0][2][2]; c2 = mat[1][0][2]; c3 = mat[2][0][0]; break; // URF
    case 3: c1 = mat[0][2][0]; c2 = mat[1][0][0]; c3 = mat[4][0][2]; break; // ULF
    case 4: c1 = mat[5][2][0]; c2 = mat[4][2][0]; c3 = mat[3][2][2]; break; // DLB
    case 5: c1 = mat[5][2][2]; c2 = mat[2][2][2]; c3 = mat[3][2][0]; break; // DRB
    case 6: c1 = mat[5][0][2]; c2 = mat[1][2][2]; c3 = mat[2][2][0]; break; // DRF
    case 7: c1 = mat[5][0][0]; c2 = mat[1][2][0]; c3 = mat[4][2][2]; break; // DLF
    default: c1 = ' '; c2 = ' '; c3 = ' '; break;
    }

    // identify the corner piece based on its colors
    int cornerId = FindCornerNum(c1, c2, c3);

    // identify the orientation of the corner piece based on the position of the white or yellow color
    int orientation = 0;
    if (c1 == 'W' || c1 == 'Y') orientation = 0;
    else if (c2 == 'W' || c2 == 'Y') orientation = 1;
    else orientation = 2;

    return { cornerId, orientation };
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
    if (this->CornerComp(a, b, c, 'W', 'B', 'O')) {
        return 0;
    }
    else if (this->CornerComp(a, b, c, 'W', 'B', 'R')) {
        return 1;
    }
    else if (this->CornerComp(a, b, c, 'W', 'G', 'R')) {
        return 2;
    }
    else if (this->CornerComp(a, b, c, 'W', 'G', 'O')) {
        return 3;
    }
    else if (this->CornerComp(a, b, c, 'Y', 'B', 'O')) {
        return 4;
    }
    else if (this->CornerComp(a, b, c, 'Y', 'B', 'R')) {
        return 5;
    }
    else if (this->CornerComp(a, b, c, 'Y', 'G', 'R')) {
        return 6;
    }
    else if (this->CornerComp(a, b, c, 'Y', 'G', 'O')) {
        return 7;
    }

    return -1;
}
void Cube::UpdateTurnForChildren_Corners(int turn, int index) {
    int current_cp[8] = {};
	int biggerCount[8] = {};
    int permutationIndex = index / 2187;

    int numbers[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };

	//calculate the count of bigger numbers for each position in the permutation from the index
    for (int i = 0; i < 8; ++i)
    {
		// calculate the count of bigger numbers for position i
        int count = permutationIndex / facts_8[i];
        permutationIndex %= facts_8[i];

		// find the actual number for position i based on the count of bigger numbers
        current_cp[i] = numbers[count];

		// manually remove the used number from the list of available numbers
        for (int j = count; j < 7 - i; ++j)
        {
            numbers[j] = numbers[j + 1];
        }
    }

	int orientationIndex = index % 2187;
	int current_co[8] = {};
    int orientationSum = 0;
    //calculate the orientation of each corner from the index
    for (int i = 0; i < 7; ++i)
    {
        current_co[i] = orientationIndex % 3;
        orientationSum += current_co[i];
        orientationIndex /= 3;
	}

    // calculate the orientation of the last corner based on the first 7 (the sum of all orientations must be divisible by 3)
	current_co[7] = (3 - (orientationSum % 3)) % 3;

	//generate child states for each possible move
    for (int m = 0; m < 18; ++m)
    {
        int child_cp[8];
        int child_co[8];

		//apply the move to the corners
        for (int i = 0; i < 8; ++i) {
            child_cp[i] = current_cp[CP_PERM[m][i]];
            child_co[i] = (current_co[CP_PERM[m][i]] + CO_ORI[m][i] + 3) % 3;
        }


		//calculate the index of the child state
		int childIndex = 0;

        int permutationIndex = 0;

        for (int i = 0; i < 8; ++i) {
            int count = 0;
            for (int j = i + 1; j < 8; ++j) {
                if (child_cp[j] < child_cp[i]) {
                    ++count;
                }
            }
            permutationIndex += (count * facts_8[i]);
        }

        int orientationIndex = 0;

        int powOf3 = 1;

        for (int i = 0; i < 7; ++i)
        {
            orientationIndex += child_co[i] * powOf3;
            powOf3 *= 3;
        }

		childIndex = permutationIndex * 2187 + orientationIndex;

		//update index in PDB if not visited before
        if (Cube::corners_pdb[childIndex] == 255) {
            Cube::corners_pdb[childIndex] = turn + 1;
        }
    }
}
int Cube::CalcCornerIndex() {
    int permutationIndex = 0;

    for (int i = 0; i < 8; ++i) {
        int count = 0;
        for (int j = i + 1; j < 8; ++j) {
            if (cp[j] < cp[i]) {
                ++count;
            }
        }
        permutationIndex += (count * facts_8[i]);
    }

	int orientationIndex = 0;

	int powOf3 = 1;

    for (int i = 0; i < 7; ++i)
    {
        orientationIndex += co[i] * powOf3;
		powOf3 *= 3;
    }

	return permutationIndex * 2187 + orientationIndex; // Final index combining permutation and orientation
}

void Cube::PDB_BFS_CORNERS() {
    bool foundNew = true;
    int turn = 0;

    while(foundNew)
    {
		foundNew = false;
        for (int i = 0; i < CORNERS_PDB_SIZE; ++i)
        {
            if (Cube::corners_pdb[i] == turn) {
                Cube::UpdateTurnForChildren_Corners(turn, i);
				foundNew = true;
            }
        }

		++turn;
    }

    FILE* f;
    errno_t err = fopen_s(&f, "corners.pdb", "wb");

    if (err == 0) {
        fwrite(corners_pdb, 1, CORNERS_PDB_SIZE, f);
        fclose(f);
    }
    else {
		cerr << "Error opening file for writing: " << err << endl;
    }

}

// edges
bool Cube::EdgeComp(char a1, char b1, char a2, char b2) {
    //sort colors
    if (a1 > b1) std::swap(a1, b1);
    if (a2 > b2) std::swap(a2, b2);

	return (a1 == a2 && b1 == b2);
}
int Cube::FindEdgeNum(char a, char b) {
	//white edges (U layer)
    if (EdgeComp(a, b, 'W', 'B')) return 0; // UB
    if (EdgeComp(a, b, 'W', 'R')) return 1; // UR
    if (EdgeComp(a, b, 'W', 'G')) return 2; // UF
    if (EdgeComp(a, b, 'W', 'O')) return 3; // UL

	//yellow edges (D layer)
    if (EdgeComp(a, b, 'Y', 'B')) return 4; // DB
    if (EdgeComp(a, b, 'Y', 'R')) return 5; // DR
    if (EdgeComp(a, b, 'Y', 'G')) return 6; // DF
    if (EdgeComp(a, b, 'Y', 'O')) return 7; // DL

	//middle layer edges
    if (EdgeComp(a, b, 'O', 'B')) return 8; // BL
    if (EdgeComp(a, b, 'R', 'B')) return 9; // BR
    if (EdgeComp(a, b, 'R', 'G')) return 10; // FR
    if (EdgeComp(a, b, 'O', 'G')) return 11; // FL

    return -1;
}
int Cube::CalcEdgesIndex_1() {
	    int occupiedPositions[6]; // indexes of the 6 edges in the first group (0-11)
	    int edgeIDs[6];           // indexes ordered by their position in the cube (0-5)
        int orientations[6];
        int found = 0;

        // סריקת כל 12 המקומות בקובייה
        for (int i = 0; i < 12; i++) {
            if (ep[i] >= 0 && ep[i] <= 5) {
                occupiedPositions[found] = i;
                edgeIDs[found] = ep[i];
                orientations[found] = eo[i];
                found++;
            }
        }

	    int combinationIndex = 0;

        for (int i = 0; i < 6; ++i)
        {
		    combinationIndex += C[occupiedPositions[i]][i + 1];
	    }

	    // calculate permutation index of the 6 edges in the first group
	    int permutationIndex = 0;

        for (int i = 0; i < 6; ++i)
        {
            int count = 0;
            for (int j = i + 1; j < 6; ++j)
            {
                if (edgeIDs[j] < edgeIDs[i]) {
                    ++count;
                }
            }
            permutationIndex += (count * facts_6[i]);
        }

        // formula for calculating the final index: combinationIndex * 720 (6!) + permutationIndex
	    int indexPos = combinationIndex * 720 + permutationIndex;

	    //calculate orientation index of the 6 edges in the first group
	    int orientationIndex = 0;
        int PowerOf2 = 1;

        for (int i = 0; i < 6; ++i)
        {
            orientationIndex += orientations[i] * PowerOf2;
            PowerOf2 *= 2;
        }

        return (combinationIndex * 720 + permutationIndex) * 64 + orientationIndex; // Final index combining position and orientation
    }
int Cube::CalcEdgesIndex_2() {
	int occupiedPositions[6]; // indexes of the 6 edges in the first group (0-11)
	int edgeIDs[6];           // indexes ordered by their position in the cube (0-5)
    int orientations[6];
    int found = 0;

    // סריקת כל 12 המקומות בקובייה
    for (int i = 0; i < 12; i++) {
        if (ep[i] >= 6 && ep[i] <= 11) {
            occupiedPositions[found] = i;
            edgeIDs[found] = ep[i] - 6;
            orientations[found] = eo[i];
            found++;
        }
    }

	int combinationIndex = 0;

    for (int i = 0; i < 6; ++i)
    {
		combinationIndex += C[occupiedPositions[i]][i + 1];
	}

	// calculate permutation index of the 6 edges in the first group
	int permutationIndex = 0;

    for (int i = 0; i < 6; ++i)
    {
        int count = 0;
        for (int j = i + 1; j < 6; ++j)
        {
            if (edgeIDs[j] < edgeIDs[i]) {
                ++count;
            }
        }
        permutationIndex += (count * facts_6[i]);
    }

    // formula for calculating the final index: combinationIndex * 720 (6!) + permutationIndex
	int indexPos = combinationIndex * 720 + permutationIndex;

	//calculate orientation index of the 6 edges in the first group
	int orientationIndex = 0;
    int PowerOf2 = 1;

    for (int i = 0; i < 6; ++i)
    {
        orientationIndex += orientations[i] * PowerOf2;
        PowerOf2 *= 2;
    }

    return (combinationIndex * 720 + permutationIndex) * 64 + orientationIndex; // Final index combining position and orientation
}

void Cube::UpdateTurnForChildren_Edges1(int turn, int index) {
	int currentOriIdx = index % 64;
    int tempPosIdx = index / 64;
    int currentPermIdx = tempPosIdx % 720;
    int currentCombIdx = tempPosIdx / 720;

	// find the edge orientation from the index
    int orientations[6] = {};

    for (int i = 0; i < 6; ++i)
    {
        orientations[i] = currentOriIdx % 2;
		currentOriIdx /= 2;
    }

	// find the ocupied edges from the index
	int ocupiedEdges[6] = {};
    int k = 6;
    for (int i = 11; i >= 0 && k > 0; --i)
    {
        if (currentCombIdx >= C[i][k]) {
            ocupiedEdges[k - 1] = i;
            currentCombIdx -= C[i][k];
            --k;
        }
    }


    //recreate the edges premutation
    int permutations[6] = {};

    int numbers[6] = {0, 1, 2, 3, 4, 5};

    int tempPerm = currentPermIdx;

    //calculate the count of bigger numbers for each position in the permutation
    for (int i = 0; i < 6; ++i)
    {
        int count = tempPerm / facts_6[i];
        tempPerm %= facts_6[i];


		permutations[i] = numbers[count];
		//manually remove the used number from the list of available numbers
        for (int j = count; j < 5 - i; ++j)
        {
            numbers[j] = numbers[j + 1];
        }
    }



    // build cube based on the premutations and orientations
    int cube_ep[12] = {};
    int cube_eo[12] = {};
    for (int i = 0; i < 12; i++) { cube_ep[i] = -1; cube_eo[i] = 0; }

    for (int i = 0; i < 6; ++i)
    {
        cube_ep[ocupiedEdges[i]] = permutations[i];
        cube_eo[ocupiedEdges[i]] = orientations[i];
    }


	//generate child states for each possible move
    for (int m = 0; m < 18; ++m) {
        int next_ep[12];
        int next_eo[12];

        // make move
        for (int i = 0; i < 12; ++i) {
            int src = EP_PERM[m][i];
            next_ep[i] = cube_ep[src];

			// update orientation if the edge is in the first group and the move affects its orientation
            if (cube_ep[src] != -1) {
                next_eo[i] = (cube_eo[src] + EO_ORI[m][i]) % 2;
            }
            else {
                next_eo[i] = -1;
            }
        }

		// identify the occupied edges, their permutations and orientations for the child state
        int next_occupied[6] = {};
        int next_permutations[6] = {};
        int next_orientations[6] = {};
        int found = 0;

        for (int i = 0; i < 12; ++i) {
            if (next_ep[i] >= 0 && next_ep[i] <= 5) {
                next_occupied[found] = i;
                next_permutations[found] = next_ep[i];
                next_orientations[found] = next_eo[i];
                ++found;
            }
        }

        // caculate the index of the child state

		//combination
        int childComb = 0;
        for (int i = 0; i < 6; ++i) {
            childComb += C[next_occupied[i]][i + 1];
        }

		// permutation
        int childPerm = 0;

        for (int i = 0; i < 6; ++i) {
            int count = 0;
            for (int j = i + 1; j < 6; ++j) {
                if (next_permutations[j] < next_permutations[i]) count++;
            }
            childPerm += count * facts_6[i];
        }

		// orientation
        int childOri = 0;
        int powerOf2 = 1;
        for (int i = 0; i < 6; i++) {
            childOri += next_orientations[i] * powerOf2;
            powerOf2 *= 2;
        }

		// final index for the child state
        int childIndex = (childComb * 720 + childPerm) * 64 + childOri;

		// update index in PDB if not visited before
        if (Cube::edges_pdb_1[childIndex] == 255) {
            Cube::edges_pdb_1[childIndex] = turn + 1;
        }
    }


}
void Cube::UpdateTurnForChildren_Edges2(int turn, int index) {
    int currentOriIdx = index % 64;
    int tempPosIdx = index / 64;
    int currentPermIdx = tempPosIdx % 720;
    int currentCombIdx = tempPosIdx / 720;

    // find the edge orientation from the index
    int orientations[6] = {};

    for (int i = 0; i < 6; ++i)
    {
        orientations[i] = currentOriIdx % 2;
        currentOriIdx /= 2;
    }

    // find the ocupied edges from the index
    int ocupiedEdges[6] = {};
    int k = 6;
    for (int i = 11; i >= 0 && k > 0; --i)
    {
        if (currentCombIdx >= C[i][k]) {
            ocupiedEdges[k - 1] = i;
            currentCombIdx -= C[i][k];
            --k;
        }
    }


    //recreate the edges premutation
    int permutations[6] = {};

    int numbers[6] = { 6, 7, 8, 9, 10, 11 };

    int tempPerm = currentPermIdx;

    //calculate the count of bigger numbers for each position in the permutation
    for (int i = 0; i < 6; ++i)
    {
        int count = tempPerm / facts_6[i];
        tempPerm %= facts_6[i];

        permutations[i] = numbers[count];

		//manually remove the used number from the list of available numbers
        for (int j = count; j < 5 - i; ++j)
        {
            numbers[j] = numbers[j + 1];
        }
    }



    // build cube based on the premutations and orientations
    int cube_ep[12] = {};
    int cube_eo[12] = {};
    for (int i = 0; i < 12; i++) { cube_ep[i] = -1; cube_eo[i] = 0; }

    for (int i = 0; i < 6; ++i)
    {
        cube_ep[ocupiedEdges[i]] = permutations[i];
        cube_eo[ocupiedEdges[i]] = orientations[i];
    }


    //generate child states for each possible move
    for (int m = 0; m < 18; ++m) {
        int next_ep[12];
        int next_eo[12];

        // make move
        for (int i = 0; i < 12; ++i) {
            int src = EP_PERM[m][i];
            next_ep[i] = cube_ep[src];

            // update orientation if the edge is in the first group and the move affects its orientation
            if (cube_ep[src] != -1) {
                next_eo[i] = (cube_eo[src] + EO_ORI[m][i]) % 2;
            }
            else {
                next_eo[i] = -1;
            }
        }

        // identify the occupied edges, their permutations and orientations for the child state
        int next_occupied[6] = {};
        int next_permutations[6] = {};
        int next_orientations[6] = {};
        int found = 0;

        for (int i = 0; i < 12; ++i) {
            if (next_ep[i] >= 6 && next_ep[i] <= 11) {
                next_occupied[found] = i;
                next_permutations[found] = next_ep[i] - 6;
                next_orientations[found] = next_eo[i];
                found++;
            }
        }

        // caculate the index of the child state

        //combination
        int childComb = 0;
        for (int i = 0; i < 6; ++i) {
            childComb += C[next_occupied[i]][i + 1];
        }

        // permutation
        int childPerm = 0;

        for (int i = 0; i < 6; ++i) {
            int count = 0;
            for (int j = i + 1; j < 6; ++j) {
                if (next_permutations[j] < next_permutations[i]) count++;
            }
            childPerm += count * facts_6[i];
        }

        // orientation
        int childOri = 0;
        int powerOf2 = 1;
        for (int i = 0; i < 6; i++) {
            childOri += next_orientations[i] * powerOf2;
            powerOf2 *= 2;
        }

        // final index for the child state
        int childIndex = (childComb * 720 + childPerm) * 64 + childOri;

        // update index in PDB if not visited before
        if (Cube::edges_pdb_2[childIndex] == 255) {
            Cube::edges_pdb_2[childIndex] = turn + 1;
        }
    }
}


void Cube::PDB_BFS_EDGES_1() {
    bool foundNew = true;
    int turn = 0;

    while (foundNew)
    {
        foundNew = false;
        for (int i = 0; i < EDGES_PDB_SIZE; ++i)
        {
            if (Cube::edges_pdb_1[i] == turn) {
                Cube::UpdateTurnForChildren_Edges1(turn, i);
                foundNew = true;
            }
        }

        ++turn;
    }

    FILE* f;
    errno_t err = fopen_s(&f, "edges_1.pdb", "wb");

    if (err == 0) {
        fwrite(edges_pdb_1, 1, EDGES_PDB_SIZE, f);
        fclose(f);
    }
    else {
        cerr << "Error opening file for writing: " << err << endl;
    }
}
void Cube::PDB_BFS_EDGES_2() {
    bool foundNew = true;
    int turn = 0;

    while (foundNew)
    {
        foundNew = false;
        for (int i = 0; i < EDGES_PDB_SIZE; ++i)
    {
            if (Cube::edges_pdb_2[i] == turn) {
                Cube::UpdateTurnForChildren_Edges2(turn, i);
                foundNew = true;
            }
        }

        ++turn;
    }

    FILE* f;
    errno_t err = fopen_s(&f, "edges_2.pdb", "wb");

    if (err == 0) {
        fwrite(edges_pdb_2, 1, EDGES_PDB_SIZE, f);
        fclose(f);
    }
    else {
        cerr << "Error opening file for writing: " << err << endl;
    }
}

pair<int, int> Cube::IdentifyEdge(const char mat[6][CUBESIZE][CUBESIZE], int position) {
    char c1, c2;

	//identify the two colors of the edge based on its position
    switch (position) {
    case 0:  c1 = mat[0][0][1]; c2 = mat[3][0][1]; break; // UB
    case 1:  c1 = mat[0][1][2]; c2 = mat[2][0][1]; break; // UR
    case 2:  c1 = mat[0][2][1]; c2 = mat[1][0][1]; break; // UF
    case 3:  c1 = mat[0][1][0]; c2 = mat[4][0][1]; break; // UL
    case 4:  c1 = mat[5][2][1]; c2 = mat[3][2][1]; break; // DB
    case 5:  c1 = mat[5][1][2]; c2 = mat[2][2][1]; break; // DR
    case 6:  c1 = mat[5][0][1]; c2 = mat[1][2][1]; break; // DF
    case 7:  c1 = mat[5][1][0]; c2 = mat[4][2][1]; break; // DL
    case 8:  c1 = mat[4][1][0]; c2 = mat[3][1][2]; break; // BL
    case 9:  c1 = mat[2][1][2]; c2 = mat[3][1][0]; break; // BR
    case 10: c1 = mat[2][1][0]; c2 = mat[1][1][2]; break; // FR
    case 11: c1 = mat[4][1][2]; c2 = mat[1][1][0]; break; // FL
    default: c1 = ' '; c2 = ' '; break;
    }

	//identify edge index based on its colors
    int edgeId = Cube::FindEdgeNum(c1, c2);

	// Determine orientation based on the position of the white or yellow sticker
    int eo = 0;
    if (c2 == 'W' || c2 == 'Y') {
        eo = 1;
    }
    else if (c1 != 'W' && c1 != 'Y' && c2 != 'W' && c2 != 'Y') {
        if (c1 == 'G' || c1 == 'B') eo = 1;
    }

    return { edgeId, eo };
}

// solution
bool Cube::IsSolved() const {
    for (int i = 0; i < 8; i++) {
        if (cp[i] != i || co[i] != 0) {
            return false;
        }
    }

    for (int i = 0; i < 12; i++) {
        if (ep[i] != i || eo[i] != 0) {
            return false;
        }
    }

    return true;
}

int Cube::GetHeuristic() {
    static PDBReader readerCorners("corners.pdb");
    static PDBReader readerEdges1("edges_1.pdb");
    static PDBReader readerEdges2("edges_2.pdb");

    int h_corners = readerCorners.GetDistance(this->CalcCornerIndex());
    int h_edges1 = readerEdges1.GetDistance(this->CalcEdgesIndex_1());
    int h_edges2 = readerEdges2.GetDistance(this->CalcEdgesIndex_2());

    int h = h_corners;
    if (h_edges1 > h) h = h_edges1;
    if (h_edges2 > h) h = h_edges2;
    return h;
}
bool Cube::SolveHelper(int turn, int limit) {
    cout << turn << ": ";
    for (int i = 0; i < turn; ++i) {
        if (steps[i] != 0) {
            cout << steps[i] << " ";
        }
    }
    cout << endl;

    if (this->IsSolved()) {
        Cube::steps[turn] = 0;
        return true;
    }


    if (turn >= limit) {
        Cube::steps[turn - 1] = 0;
        return false;
    }

    priority_queue<pair<int, int>, vector<pair<int, int>>, PairFirstComparator> minHeap_States;
    int h;

    int move;


    if (turn == 0 || (turn > 0 && (steps[turn - 1] - 1) / 3 != 0)) {
        //check for R
        this->R();

        h = GetHeuristic();

        move = 1;
        minHeap_States.push(make_pair(h, move));

        this->R_P();

        //check for R_P
        this->R_P();

        h = GetHeuristic();

        move = 2;
        minHeap_States.push(make_pair(h, move));

        this->R();

        //check for R2
        this->R2();

        h = GetHeuristic();

        move = 3;
        minHeap_States.push(make_pair(h, move));

        this->R2();

    }

    if (turn == 0 || (turn > 0 && (steps[turn - 1] - 1) / 3 != 1)) {
        //check for L
        this->L();

        h = GetHeuristic();

        move = 4;
        minHeap_States.push(make_pair(h, move));

        this->L_P();

        //check for L_P
        this->L_P();

        h = GetHeuristic();

        move = 5;
        minHeap_States.push(make_pair(h, move));

        this->L();

        //check for L2
        this->L2();

        h = GetHeuristic();

        move = 6;
        minHeap_States.push(make_pair(h, move));

        this->L2();

    }

    if (turn == 0 || (turn > 0 && (steps[turn - 1] - 1) / 3 != 2)) {
        //check for U
        this->U();

        h = GetHeuristic();

        move = 7;
        minHeap_States.push(make_pair(h, move));

        this->U_P();

        //check for U_P
        this->U_P();

        h = GetHeuristic();

        move = 8;
        minHeap_States.push(make_pair(h, move));

        this->U();

        //check for U2
        this->U2();

        h = GetHeuristic();

        move = 9;
        minHeap_States.push(make_pair(h, move));

        this->U2();

    }

    if (turn == 0 || (turn > 0 && (steps[turn - 1] - 1) / 3 != 3)) {
        //check for D
        this->D();

        h = GetHeuristic();

        move = 10;
        minHeap_States.push(make_pair(h, move));

        this->D_P();

        //check for D_P
        this->D_P();

        h = GetHeuristic();

        move = 11;
        minHeap_States.push(make_pair(h, move));

        this->D();

        //check for D2
        this->D2();

        h = GetHeuristic();

        move = 12;
        minHeap_States.push(make_pair(h, move));

        this->D2();

    }

    if (turn == 0 || (turn > 0 && (steps[turn - 1] - 1) / 3 != 4)) {
        //check for F
        this->F();

        h = GetHeuristic();

        move = 13;
        minHeap_States.push(make_pair(h, move));

        this->F_P();

        //check for F_P
        this->F_P();

        h = GetHeuristic();

        move = 14;
        minHeap_States.push(make_pair(h, move));

        this->F();

        //check for F2
        this->F2();

        h = GetHeuristic();

        move = 15;
        minHeap_States.push(make_pair(h, move));

        this->F2();

    }

    if (turn == 0 || (turn > 0 && (steps[turn - 1] - 1) / 3 != 5)) {
        //check for B
        this->B();

        h = GetHeuristic();

        move = 16;
        minHeap_States.push(make_pair(h, move));

        this->B_P();

        //check for B_P
        this->B_P();

        h = GetHeuristic();

        move = 17;
        minHeap_States.push(make_pair(h, move));

        this->B();

        //check for B2
        this->B2();

        h = GetHeuristic();

        move = 18;
        minHeap_States.push(make_pair(h, move));

        this->B2();

    }


    while (!minHeap_States.empty()) {
        int _move = minHeap_States.top().second;
		int h = minHeap_States.top().first;
        minHeap_States.pop();

        if (turn + h >= limit) {
			return false; // Prune branches that cannot lead to a solution within the depth limit
        }

        switch (_move) {
        case 1: this->R(); break;
        case 2: this->R_P(); break;
        case 3: this->R2(); break;
        case 4: this->L(); break;
        case 5: this->L_P(); break;
        case 6: this->L2(); break;
        case 7: this->U(); break;
        case 8: this->U_P(); break;
        case 9: this->U2(); break;
        case 10: this->D(); break;
        case 11: this->D_P(); break;
        case 12: this->D2(); break;
        case 13: this->F(); break;
        case 14: this->F_P(); break;
        case 15: this->F2(); break;
        case 16: this->B(); break;
        case 17: this->B_P(); break;
        case 18: this->B2(); break;
        }


        steps[turn] = _move;

        if (this->SolveHelper(turn + 1, limit)) {
            return true;
        }
        else {
            // Undo move
            switch (_move)
            {
            case 1: this->R_P(); break;
            case 2: this->R(); break;
            case 3: this->R2(); break;
            case 4: this->L_P(); break;
            case 5: this->L(); break;
            case 6: this->L2(); break;
            case 7: this->U_P(); break;
            case 8: this->U(); break;
            case 9: this->U2(); break;
            case 10: this->D_P(); break;
            case 11: this->D(); break;
            case 12: this->D2(); break;
            case 13: this->F_P(); break;
            case 14: this->F(); break;
            case 15: this->F2(); break;
            case 16: this->B_P(); break;
            case 17: this->B(); break;
            case 18: this->B2(); break;
            }
        }
    }
    
    return false;
}

vector<int> Cube::Solve(int limit) {
	Cube::InitPdb();
	//Cube().PDB_BFS_CORNERS();
    //Cube().PDB_BFS_EDGES_1();
    //Cube().PDB_BFS_EDGES_2();

    if (SolveHelper(0, limit)) {
        return steps;
    }
    else {
        return {0};
    }

	return {0};
}