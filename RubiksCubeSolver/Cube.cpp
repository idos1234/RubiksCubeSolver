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

    Cube::corners_pdb[0] = 0;
	Cube::edges_pdb_1[0] = 0;
    Cube::edges_pdb_2[0] = 0;


}

void Cube::ManualApplyMove(int face) {
    // עזרים לביצוע החלפות (Swaps)
    auto rotateCorners = [&](int c1, int c2, int c3, int c4) {
        signed char temp_cp = cp[c1]; cp[c1] = cp[c2]; cp[c2] = cp[c3]; cp[c3] = cp[c4]; cp[c4] = temp_cp;
        signed char temp_co = co[c1]; co[c1] = co[c2]; co[c2] = co[c3]; co[c3] = co[c4]; co[c4] = temp_co;
        };
    auto rotateEdges = [&](int e1, int e2, int e3, int e4) {
        signed char temp_ep = ep[e1]; ep[e1] = ep[e2]; ep[e2] = ep[e3]; ep[e3] = ep[e4]; ep[e4] = temp_ep;
        signed char temp_eo = eo[e1]; eo[e1] = eo[e2]; eo[e2] = eo[e3]; eo[e3] = eo[e4]; eo[e4] = temp_eo;
        };

    switch (face) {
    case 0: // U
        rotateCorners(0, 1, 2, 3);
        rotateEdges(0, 1, 2, 3);
        break;
    case 1: // D
        rotateCorners(4, 7, 6, 5);
        rotateEdges(4, 7, 6, 5);
        break;
    case 2: // L
        rotateCorners(0, 3, 4, 5);
        rotateEdges(8, 11, 4, 9);
		// change corner orientations for L moves (clockwise rotation adds 1 to the orientation of the affected corners, counter-clockwise adds 2)
        co[0] = (co[0] + 1) % 3; co[3] = (co[3] + 2) % 3;
        co[4] = (co[4] + 1) % 3; co[5] = (co[5] + 2) % 3;
		// change edge orientations for L moves (edges in the L face change orientation)
        break;
    case 3: // R
        rotateCorners(1, 6, 7, 2);
        rotateEdges(2, 6, 10, 1);
        co[1] = (co[1] + 1) % 3; co[6] = (co[6] + 2) % 3;
        co[7] = (co[7] + 1) % 3; co[2] = (co[2] + 2) % 3;
        break;
    case 4: // F
        rotateCorners(2, 7, 4, 3);
        rotateEdges(3, 7, 11, 8);
        co[2] = (co[2] + 1) % 3; co[7] = (co[7] + 2) % 3;
        co[4] = (co[4] + 1) % 3; co[3] = (co[3] + 2) % 3;
		// change edge orientations for F moves (edges in the F face change orientation)
        for (int e : {3, 7, 11, 8}) eo[e] = 1 - eo[e];
        break;
    case 5: // B
        rotateCorners(0, 5, 6, 1);
        rotateEdges(1, 5, 9, 6);
        co[0] = (co[0] + 1) % 3; co[5] = (co[5] + 2) % 3;
        co[6] = (co[6] + 1) % 3; co[1] = (co[1] + 2) % 3;
        for (int e : {1, 5, 9, 6}) eo[e] = 1 - eo[e];
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

Cube::Cube(const char cubeMat[6][CUBESIZE][CUBESIZE]) :numOfTurns(0), steps(20, nullptr) {
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

Cube::Cube() :numOfTurns(0), steps(20, nullptr) {
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

	//update corners in one step
    for (int i = 0; i < 8; i++) {
		int src = CP_PERM[moveIdx][i]; //from the move table, get the source index for this corner
        next_cp[i] = cp[src];
		// update orientation (add the value from the table modulo 3)
        next_co[i] = (co[src] + CO_ORI[moveIdx][i]) % 3;
    }

	// update edges in one step
    for (int i = 0; i < 12; i++) {
        int src = EP_PERM[moveIdx][i];
        next_ep[i] = ep[src];
		// update orientation (add the value from the table modulo 2)
        next_eo[i] = (eo[src] + EO_ORI[moveIdx][i]) % 2;
    }

	// copy back the results to the cube's state
    for (int i = 0; i < 8; i++) { cp[i] = next_cp[i]; co[i] = next_co[i]; }
    for (int i = 0; i < 12; i++) { ep[i] = next_ep[i]; eo[i] = next_eo[i]; }
}

// --- R Moves (Right face - Red) ---
void Cube::R() {
	this->ApplyMove(2, 1);
}
void Cube::R_P() {
    this->ApplyMove(2, 2);
}
void Cube::R2() {
    this->ApplyMove(2, 3);
}

// --- L Moves (Left face - Orange) ---
void Cube::L() {
    this->ApplyMove(4, 1);
}
void Cube::L_P() {
    this->ApplyMove(4, 2);
}
void Cube::L2() {
    this->ApplyMove(4, 3);
}

// --- U Moves (Up face - White) ---
void Cube::U() {
    this->ApplyMove(0, 1);
}
void Cube::U_P() {
    this->ApplyMove(0, 2);
}
void Cube::U2() {
    this->ApplyMove(0, 3);
}

// --- D Moves (Down face - Yellow) ---
void Cube::D() {
    this->ApplyMove(5, 1);
}
void Cube::D_P() {
    this->ApplyMove(5, 2);
}
void Cube::D2() {
    this->ApplyMove(5, 3);
}

// --- F Moves (Front face - Green) ---
void Cube::F() {
    this->ApplyMove(1, 1);
}
void Cube::F_P() {
    this->ApplyMove(1, 2);
}
void Cube::F2() {
    this->ApplyMove(1, 3);
}

// --- B Moves (Back face - Blue) ---
void Cube::B() {
    this->ApplyMove(3, 1);
}
void Cube::B_P() {
    this->ApplyMove(3, 2);
}
void Cube::B2() {
    this->ApplyMove(3, 3);
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
void Cube::UpdateTurnForChildren_Corners(int turn, int index) {
    int indexes[8] = {};
	int biggerCount[8] = {};

    int numbers[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };

	//calculate the count of bigger numbers for each position in the permutation from the index
    for (int i = 0; i < 8; ++i)
    {
		// calculate the count of bigger numbers for position i
        int count = index / facts_8[i];
        index %= facts_8[i];

        // השמה של הפינה הנכונה במיקום i
        indexes[i] = numbers[count];

		// manually remove the used number from the list of available numbers
        for (int j = count; j < 7 - i; ++j)
        {
            numbers[j] = numbers[j + 1];
        }
    }

	//generate child states for each possible move
    for (int m = 0; m < 18; ++m)
    {
        int child_cp[8];

		//apply the move to the corners
        for (int i = 0; i < 8; ++i) {
            child_cp[i] = indexes[CP_PERM[m][i]];
        }


		//calculate the index of the child state
		int childIndex = 0;

        for (int i = 0; i < 8; ++i)
        {
            int count = 0;

            for (int j = i + 1; j < 8; ++j)
            {
                if (child_cp[j] < child_cp[i]) {
                    ++count;
                }
            }

            childIndex += (count * facts_8[i]);
        }

		//update index in PDB if not visited before
        if (Cube::corners_pdb[childIndex] == 255) {
            Cube::corners_pdb[childIndex] = turn + 1;
        }
    }
}
int Cube::CalcCornerPermutationIndex() {
    int index = 0;

    for (int i = 0; i < 8; ++i) {
        int count = 0;
        for (int j = i + 1; j < 8; ++j) {
            if (cp[j] < cp[i]) {
                ++count;
            }
        }
        index += (count * facts_8[i]);
    }

    return index;
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
        if (c2 == 'G' || c2 == 'B') eo = 1;
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
    int h_corners = Cube::corners_pdb[CalcCornerPermutationIndex()];
    int h_edges1 = Cube::edges_pdb_1[CalcEdgesIndex_1()];
    int h_edges2 = Cube::edges_pdb_2[CalcEdgesIndex_2()];

    return std::max({ h_corners, h_edges1, h_edges2 });
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
    int h;

    char move[3];


    //check for R
	this->R();

    h = GetHeuristic();

	move[0] = 'R';
	move[1] = '\0';
	minHeap_States.push(make_pair(h, _strdup(move)));

	this->R_P();

    //check for R_P
    this->R_P();

    h = GetHeuristic();

    move[0] = 'R';
    move[1] = '\'';
    move[2] = '\0';
	minHeap_States.push(make_pair(h, _strdup(move)));

    this->R();

    //check for R2
    this->R2();

    h = GetHeuristic();

	move[0] = 'R';
	move[1] = '2';
	move[2] = '\0';
	minHeap_States.push(make_pair(h, _strdup(move)));

    this->R2();

    //check for L
    this->L();

    h = GetHeuristic();

	move[0] = 'L';
	move[1] = '\0';
	minHeap_States.push(make_pair(h, _strdup(move)));

    this->L_P();

    //check for L_P
    this->L_P();

    h = GetHeuristic();

    move[0] = 'L';
    move[1] = '\'';
    move[2] = '\0';
	minHeap_States.push(make_pair(h, _strdup(move)));

    this->L();

    //check for L2
    this->L2();

    h = GetHeuristic();

	move[0] = 'L';
	move[1] = '2';
	move[2] = '\0';
	minHeap_States.push(make_pair(h, _strdup(move)));

    this->L2();

    //check for U
    this->U();

    h = GetHeuristic();

	move[0] = 'U';
	move[1] = '\0';
	minHeap_States.push(make_pair(h, _strdup(move)));

    this->U_P();

    //check for U_P
    this->U_P();

    h = GetHeuristic();

    move[0] = 'U';
    move[1] = '\'';
    move[2] = '\0';
	minHeap_States.push(make_pair(h, _strdup(move)));

    this->U();

    //check for U2
    this->U2();

    h = GetHeuristic();

	move[0] = 'U';
	move[1] = '2';
	move[2] = '\0';
	minHeap_States.push(make_pair(h, _strdup(move)));

    this->U2();

    //check for D
    this->D();

    h = GetHeuristic();

	move[0] = 'D';
	move[1] = '\0';
	minHeap_States.push(make_pair(h, _strdup(move)));

    this->D_P();

    //check for D_P
    this->D_P();

    h = GetHeuristic();

    move[0] = 'D';
    move[1] = '\'';
    move[2] = '\0';
	minHeap_States.push(make_pair(h, _strdup(move)));

    this->D();

    //check for D2
    this->D2();

    h = GetHeuristic();

	move[0] = 'D';
	move[1] = '2';
	move[2] = '\0';
    minHeap_States.push(make_pair(h, _strdup(move)));

    this->D2();

    //check for B
    this->B();

    h = GetHeuristic();

	move[0] = 'B';
	move[1] = '\0';
	minHeap_States.push(make_pair(h, _strdup(move)));

    this->B_P();

    //check for B_P
    this->B_P();

    h = GetHeuristic();

    move[0] = 'B';
    move[1] = '\'';
    move[2] = '\0';
	minHeap_States.push(make_pair(h, _strdup(move)));

    this->B();

    //check for B2
    this->B2();

    h = GetHeuristic();

	move[0] = 'B';
	move[1] = '2';
    move[2] = '\0';
	minHeap_States.push(make_pair(h, _strdup(move)));

    this->B2();

    //check for F
    this->F();

    h = GetHeuristic();

	move[0] = 'F';
	move[1] = '\0';
	minHeap_States.push(make_pair(h, _strdup(move)));

    this->F_P();

    //check for F_P
    this->F_P();

    h = GetHeuristic();

    move[0] = 'F';
    move[1] = '\'';
    move[2] = '\0';
	minHeap_States.push(make_pair(h, _strdup(move)));

    this->F();

    //check for F2
    this->F2();

    h = GetHeuristic();

	move[0] = 'F';
	move[1] = '2';
	move[2] = '\0';
    minHeap_States.push(make_pair(h, _strdup(move)));

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
	Cube().PDB_BFS_CORNERS();
    Cube().PDB_BFS_EDGES_1();
    Cube().PDB_BFS_EDGES_2();

    if (SolveHelper(0)) {
        return steps;
    }
    else {
        return {"No solution found"};
    }


}