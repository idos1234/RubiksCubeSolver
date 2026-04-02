#include "cube.h"

unsigned char* Cube::corners_pdb = nullptr;
unsigned char* Cube::edges_pdb_1 = nullptr;
unsigned char* Cube::edges_pdb_2 = nullptr;

signed char Cube::EP_PERM[18][12];
signed char Cube::EO_ORI[18][12];
signed char Cube::CP_PERM[18][8];
signed char Cube::CO_ORI[18][8];

int Cube::CP_MoveTable[40320][18];
int Cube::CO_MoveTable[2187][18];
int (*Cube::Edges1_MoveTable)[18] = nullptr;
int (*Cube::Edges2_MoveTable)[18] = nullptr;

const int Cube::facts_8[8] = { 5040, 720, 120, 24, 6, 2, 1, 0 };
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

void Cube::LoadPdb()
{
	// initialize the solved state in the PDBs

    const size_t sizes[] = { CORNERS_PDB_SIZE, EDGES_PDB_SIZE, EDGES_PDB_SIZE };
    const char* files[] = { "corners.pdb", "edges_1.pdb", "edges_2.pdb" };
    unsigned char** targets[] = { &corners_pdb, &edges_pdb_1, &edges_pdb_2 };

    for (int i = 0; i < 3; i++) {
        if (*targets[i] == nullptr) {
			// Attempt to open the PDB file
            FILE* f;
            if (fopen_s(&f, files[i], "rb") != 0 || f == nullptr) {
                cerr << "Failed to open PDB file! - " << files[i] << " (Make sure it exists in the exe folder)" << endl;
                exit(EXIT_FAILURE);
            }

			// Allocate memory for the PDB
            *targets[i] = (unsigned char*)malloc(sizes[i]);
            if (*targets[i] == nullptr) {
                fclose(f);
                cerr << "Failed to allocate memory for PDB! - " << files[i] << endl;
                exit(EXIT_FAILURE);
            }

			// Read the PDB data into memory
            size_t bytesRead = fread(*targets[i], 1, sizes[i], f);

            fclose(f);

            if (bytesRead != sizes[i]) {
                cerr << "Warning: PDB file " << files[i] << " seems corrupted or wrong size!" << endl;
            }
        }
    }
}
void Cube::CreatePdb() {
    const size_t sizes[] = { CORNERS_PDB_SIZE, EDGES_PDB_SIZE, EDGES_PDB_SIZE };
    const char* files[] = { "corners.pdb", "edges_1.pdb", "edges_2.pdb" };
    unsigned char** targets[] = { &corners_pdb, &edges_pdb_1, &edges_pdb_2 };
    Cube solvedCube;
	int solvedIndex = 0;


    for (int i = 0; i < 3; i++) {
        if (*targets[i] == nullptr) {
            *targets[i] = (unsigned char*)malloc(sizes[i]);
            if (*targets[i] == nullptr) {
                cerr << "Failed to allocate memory for PDB! - " << files[i] << endl;
                exit(EXIT_FAILURE);
            }

			// Initialize all entries to 255 (indicating unsolved)
            memset(*targets[i], 255, sizes[i]);

            if (i == 2) {
				solvedIndex = solvedCube.CalcEdgesIndex_2();
            }
            (*targets[i])[solvedIndex] = 0;
        }
	}
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
        setC(3, 2, 1); setC(2, 6, 2); setC(6, 7, 1); setC(7, 3, 2);
        setE(2, 5, true); setE(5, 10, true); setE(10, 6, true); setE(6, 2, true);
        break;

    case 2: // R (Right)
        setC(1, 2, 1); setC(2, 6, 2); setC(6, 5, 1); setC(5, 1, 2);
        setE(1, 2, false); setE(2, 6, false); setE(6, 5, false); setE(5, 1, false);
        break;

    case 3: // B (Back)
        setC(0, 4, 1); setC(4, 5, 2); setC(5, 1, 1); setC(1, 0, 2);
        setE(0, 4, true); setE(4, 9, true); setE(9, 1, true); setE(1, 0, true);
        break;

    case 4: // L (Left)
        setC(0, 3, 1); setC(3, 7, 2); setC(7, 4, 1); setC(4, 0, 2);
        setE(3, 7, false); setE(7, 11, false); setE(11, 4, false); setE(4, 3, false);
        break;

    case 5: // D (Down)
        setC(4, 7, 0); setC(7, 6, 0); setC(6, 5, 0); setC(5, 4, 0);
        setE(4, 7, false); setE(7, 6, false); setE(6, 5, false); setE(5, 4, false);
        break;
    }
}
void Cube::InitMoveTables() {
    LoadEdgesMoveTables();
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

void Cube::GenerateCPMoveTable() {
    Cube cube;

    for (int i = 0; i < 40320; i++) {
        // set cube state by index
        cube.DecodeCornerPermutation(i);

        for (int move = 0; move < 18; move++) {
            // do move
            int face = move / 3;
            int times = (move % 3) + 1;
            cube.ApplyMove(face, times);

            // save new index in the table
            CP_MoveTable[i][move] = cube.CalcCornerPermutationIndex();

			//undo move
			cube.ApplyMove(face, 4 - times);
        }
    }
}
void Cube::GenerateCOMoveTable() {
    Cube cube;

    for (int i = 0; i < 2187; i++) {
        // set cube state by index
        cube.DecodeCornerOrientation(i);

        for (int move = 0; move < 18; move++) {
            // do move
            int face = move / 3;
            int times = (move % 3) + 1;
            cube.ApplyMove(face, times);

            // save new index in the table
            CO_MoveTable[i][move] = cube.CalcCornerOrientationIndex();

            // undo move
			cube.ApplyMove(face, 4 - times);
        }
    }
}

void Cube::GenerateEdges1MoveTable() {
    Cube cube;

    for (int i = 0; i < EDGES_PDB_SIZE; ++i)
    {
		cube.DecodeEdges_1(i);

        for (int move = 0; move < 18; ++move)
        {
            int face = move / 3;
            int times = (move % 3) + 1;
            cube.ApplyMove(face, times);

			int newIndex = cube.CalcEdgesIndex_1();

            Edges1_MoveTable[i][move] = newIndex;

            // undo move
			cube.ApplyMove(face, 4 - times);
        }
    }

    FILE* f;
    errno_t err = fopen_s(&f, "edges1_move.table", "wb");

    if (err == 0) {
        fwrite(Edges1_MoveTable, sizeof(int) * 18, EDGES_PDB_SIZE, f);
        fclose(f);
    }
    else {
        cerr << "Error opening file for writing: " << err << endl;
    }
}
void Cube::GenerateEdges2MoveTable() {
    Cube cube;

    for (int i = 0; i < EDGES_PDB_SIZE; ++i)
    {
		cube.DecodeEdges_2(i);

        for (int move = 0; move < 18; ++move)
        {
            int face = move / 3;
            int times = (move % 3) + 1;
            cube.ApplyMove(face, times);

			int newIndex = cube.CalcEdgesIndex_2();

            Edges2_MoveTable[i][move] = newIndex;

            // undo move
			cube.ApplyMove(face, 4 - times);
        }
    }

    FILE* f;
    errno_t err = fopen_s(&f, "edges2_move.table", "wb");

    if (err == 0) {
        fwrite(Edges2_MoveTable, sizeof(int) * 18, EDGES_PDB_SIZE, f);
        fclose(f);
    }
    else {
        cerr << "Error opening file for writing: " << err << endl;
    }
}

void Cube::LoadEdgesMoveTables() {
    FILE* f1;
    if (fopen_s(&f1, "edges1_move.table", "rb") != 0) exit(EXIT_FAILURE);

    FILE* f2;
    if (fopen_s(&f2, "edges2_move.table", "rb") != 0) {
        fclose(f1);
        exit(EXIT_FAILURE);
    }

    cout << "Loading Move Tables from disk..." << endl;

    // הקצאת זיכרון ב-Heap
    Edges1_MoveTable = (int (*)[18])malloc((long long)EDGES_PDB_SIZE * 18 * sizeof(int));
    Edges2_MoveTable = (int (*)[18])malloc((long long)EDGES_PDB_SIZE * 18 * sizeof(int));

    if (!Edges1_MoveTable || !Edges2_MoveTable) {
        cerr << "Memory allocation failed during load!" << endl;
        exit(EXIT_FAILURE);
    }

    fread(Edges1_MoveTable, sizeof(int) * 18, EDGES_PDB_SIZE, f1);
    fread(Edges2_MoveTable, sizeof(int) * 18, EDGES_PDB_SIZE, f2);

    fclose(f1);
    fclose(f2);
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
            if (type == 0) cout << "U "; else if (type == 1) cout << "U' "; else cout << "U2 ";
            break;
        case 1: // Front (Green)
            if (type == 0) F(); else if (type == 1) F_P(); else F2();
            if (type == 0) cout << "F "; else if (type == 1) cout << "F' "; else cout << "F2 ";
            break;
        case 2: // Right (Red)
            if (type == 0) R(); else if (type == 1) R_P(); else R2();
            if (type == 0) cout << "R "; else if (type == 1) cout << "R' "; else cout << "R2 ";
            break;
        case 3: // Back (Blue)
            if (type == 0) B(); else if (type == 1) B_P(); else B2();
            if (type == 0) cout << "B "; else if (type == 1) cout << "B' "; else cout << "B2 ";
            break;
        case 4: // Left (Orange)
            if (type == 0) L(); else if (type == 1) L_P(); else L2();
            if (type == 0) cout << "L "; else if (type == 1) cout << "L' "; else cout << "L2 ";
            break;
        case 5: // Down (Yellow)
            if (type == 0) D(); else if (type == 1) D_P(); else D2();
            if (type == 0) cout << "D "; else if (type == 1) cout << "D' "; else cout << "D2 ";
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
int Cube::CalcCornerIndex() {
    return (CalcCornerPermutationIndex() * 2187) + CalcCornerOrientationIndex();
}

void Cube::DecodeCornerOrientation(int index) {
    int orientationSum = 0;

    for (int i = 0; i < 7; ++i)
    {
        co[i] = index % 3;
        orientationSum += co[i];
        index /= 3;
    }

    co[7] = (3 - (orientationSum % 3)) % 3;
}
int Cube::CalcCornerOrientationIndex() {
    int orientationIndex = 0;
    int powOf3 = 1;

    for (int i = 0; i < 7; ++i)
    {
        orientationIndex += co[i] * powOf3;
        powOf3 *= 3;
    }

    return orientationIndex;
}

void Cube::DecodeCornerPermutation(int index) {
    int corners[8] = {0, 1, 2, 3, 4, 5, 6, 7};

    for (int i = 0; i < 7; ++i)
    {
        int pos = index / facts_8[i];
		cp[i] = corners[pos];
		index %= facts_8[i];

		// delete the used corner from the list of available corners
        for (int j = pos; j < 7 - i; j++) {
            corners[j] = corners[j + 1];
        }
    }

    cp[7] = corners[0];
}
int Cube::CalcCornerPermutationIndex() {
    int permutationIndex = 0;

    for (int i = 0; i < 7; ++i) {
        int count = 0;
        for (int j = i + 1; j < 8; ++j) {
            if (cp[j] < cp[i]) {
                ++count;
            }
        }
        permutationIndex += (count * facts_8[i]);
    }

    return permutationIndex;
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
                int cpIdx = i / 2187;
                int coIdx = i % 2187;

				// update childern states based on the move tables
                for (int move = 0; move < 18; ++move) {
                    int nextCP = CP_MoveTable[cpIdx][move];
                    int nextCO = CO_MoveTable[coIdx][move];

                    int childIndex = (nextCP * 2187) + nextCO;
                        
					// if this state hasn't been reached before, set its value to the current turn + 1 and mark that we found a new state
                    if (Cube::corners_pdb[childIndex] == 255) {
                        Cube::corners_pdb[childIndex] = (unsigned char)(turn + 1);
                        foundNew = true;
                    }
                }
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

void Cube::ResetCubeEdges() {
    for (int i = 0; i < 12; i++) {
        ep[i] = -1;
        eo[i] = 0;
    }
}

int Cube::CalcEdgesIndex_1() {
    return (CalcEdgePositionIndex_1() * 720 * 64) + (CalcEdgePermutationIndex_1() * 64) + CalcEdgeOrientationIndex_1();
}
void Cube::DecodeEdges_1(int fullIndex) {
	ResetCubeEdges();

    int oriIndex = fullIndex % 64;
    int permIndex = (fullIndex / 64) % 720;
    int posIndex = fullIndex / (64 * 720);

    int activePositions[6];
    DecodeEdgePosition_1(posIndex, activePositions);
    DecodeEdgePermutation_1(permIndex, activePositions);
    DecodeEdgeOrientation_1(oriIndex, activePositions);
}

int Cube::CalcEdgesIndex_2() {
    return (CalcEdgePositionIndex_2() * 720 * 64) + (CalcEdgePermutationIndex_2() * 64) + CalcEdgeOrientationIndex_2();
}
void Cube::DecodeEdges_2(int fullIndex) {
	ResetCubeEdges();

    int oriIndex = fullIndex % 64;
    int permIndex = (fullIndex / 64) % 720;
    int posIndex = fullIndex / (64 * 720);

    int activePositions[6];
    DecodeEdgePosition_2(posIndex, activePositions);
    DecodeEdgePermutation_2(permIndex, activePositions);
    DecodeEdgeOrientation_2(oriIndex, activePositions);
}

int Cube::CalcEdgeOrientationIndex_1() {
    int orientations[12] = {};
    int orientationIndex = 0;
    int PowerOf2 = 1;
    int found = 0;

    for (int i = 0; i < 12; i++) {
        if (ep[i] >= 0 && ep[i] <= 5) {
            if (found < 6) {
                orientations[found] = eo[i];
            }
            found++;
        }
    }

    for (int i = 0; i < 6; ++i)
    {
        orientationIndex += orientations[i] * PowerOf2;
        PowerOf2 *= 2;
    }

    return orientationIndex;
}
void Cube::DecodeEdgeOrientation_1(int index, int* activePositions) {
    for (int i = 0; i < 6; ++i)
    {
        int cubePos = activePositions[i];
        eo[cubePos] = index % 2;
        index /= 2;
    }
}

int Cube::CalcEdgePermutationIndex_1() {
    int permutationIndex = 0;
    int edgeIDs[12] = {};           // indexes ordered by their position in the cube (0-5)
    int found = 0;

    for (int i = 0; i < 12; i++) {
        if (ep[i] >= 0 && ep[i] <= 5) {
            edgeIDs[found] = ep[i];
            found++;
        }
    }

    for (int i = 0; i < 5; ++i)
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

    return permutationIndex;
}
void Cube::DecodeEdgePermutation_1(int index, int* activePositions) {
	int availableEdges[6] = { 0, 1, 2, 3, 4, 5 };

    for (int i = 0; i < 5; i++) {
        int count = index / facts_6[i];
        index %= facts_6[i];

        int actualCubePos = activePositions[i];
        ep[actualCubePos] = availableEdges[count];

        for (int j = count; j < 5 - i; j++) {
            availableEdges[j] = availableEdges[j + 1];
        }
    }

    ep[activePositions[5]] = availableEdges[0];
}

int Cube::CalcEdgePositionIndex_1() {
    int positionIndex = 0;
    int k = 6;
    for (int i = 11; i >= 0 && k > 0; i--) {
        if (ep[i] >= 0 && ep[i] <= 5) {
            positionIndex += C[i][k];
            k--;
        }
    }
    return positionIndex;
}
void Cube::DecodeEdgePosition_1(int index, int* activePositions) {
    int k = 6;

    for (int i = 11; i >= 0 && k > 0; --i)
    {
        if (C[i][k] <= index)
        {
            activePositions[k - 1] = i;
            index -= C[i][k];
            --k;
        }
    }

    sort(activePositions, activePositions + 6);
}

int Cube::CalcEdgeOrientationIndex_2() {
    int orientations[12] = {};
    int orientationIndex = 0;
    int PowerOf2 = 1;
    int found = 0;

    for (int i = 0; i < 12; i++) {
        if (ep[i] >= 6 && ep[i] <= 11) {
            if (found < 6) {
                orientations[found] = eo[i];
            }
            found++;
        }
    }

    for (int i = 0; i < 6; ++i)
    {
        orientationIndex += orientations[i] * PowerOf2;
        PowerOf2 *= 2;
    }

    return orientationIndex;
}
void Cube::DecodeEdgeOrientation_2(int index, int* activePositions) {
    for (int i = 0; i < 6; ++i)
    {
        int cubePos = activePositions[i];
        eo[cubePos] = index % 2;
        index /= 2;
    }
}


int Cube::CalcEdgePermutationIndex_2() {
    int permutationIndex = 0;
    int edgeIDs[12] = {};           // indexes ordered by their position in the cube (6-11)
    int found = 0;

    for (int i = 0; i < 12; i++) {
        if (ep[i] >= 6 && ep[i] <= 11) {
            edgeIDs[found] = ep[i];
            found++;
        }
    }

    for (int i = 0; i < 5; ++i)
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

    return permutationIndex;
}
void Cube::DecodeEdgePermutation_2(int index, int* activePositions) {
    int availableEdges[6] = { 6, 7, 8, 9, 10, 11 };

    for (int i = 0; i < 5; i++) {
        int count = index / facts_6[i];
        index %= facts_6[i];

        int actualCubePos = activePositions[i];
        ep[actualCubePos] = availableEdges[count];

        for (int j = count; j < 5 - i; j++) {
            availableEdges[j] = availableEdges[j + 1];
        }
    }

	ep[activePositions[5]] = availableEdges[0];
}


int Cube::CalcEdgePositionIndex_2() {
    int positionIndex = 0;
    int k = 6;
    for (int i = 11; i >= 0 && k > 0; i--) {
        if (ep[i] >= 6 && ep[i] <= 11) {
            positionIndex += C[i][k];
            k--;
        }
    }
    return positionIndex;
}
void Cube::DecodeEdgePosition_2(int index, int* activePositions) {
    int k = 6;

    for (int i = 11; i >= 0 && k > 0; --i)
    {
        if (C[i][k] <= index)
        {
            activePositions[k - 1] = i;
            index -= C[i][k];
            --k;
        }
    }

    sort(activePositions, activePositions + 6);
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

                for (int move = 0; move < 18; ++move)
                {
                    int nextIndex = Edges1_MoveTable[i][move];
                    if (Cube::edges_pdb_1[nextIndex] == 255) {
                        Cube::edges_pdb_1[nextIndex] = (unsigned char)(turn + 1);
                    }
				}

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
                
                for (int move = 0; move < 18; ++move)
                {
                    int nextIndex = Edges2_MoveTable[i][move];
                    if (Cube::edges_pdb_2[nextIndex] == 255) {
                        Cube::edges_pdb_2[nextIndex] = (unsigned char)(turn + 1);
                    }
				}

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

int Cube::GetHeuristic(int cpIdx, int coIdx, int e1Idx, int e2Idx) {
    int h_corners = corners_pdb[cpIdx * 2187 + coIdx];
    int h_edges1 = edges_pdb_1[e1Idx];
    int h_edges2 = edges_pdb_2[e2Idx];

    int h = h_corners;
    if (h_edges1 > h) h = h_edges1;
    if (h_edges2 > h) h = h_edges2;
    return h;
}

bool Cube::ShouldSkipMove(int currentFace, int lastFace) {
    if (currentFace == lastFace) return true;
	// skip moves that undo the last move (e.g., if the last move was on face 0, skip moves on face 5, etc.)
    if ((currentFace == 0 && lastFace == 5) ||
        (currentFace == 1 && lastFace == 3) ||
        (currentFace == 2 && lastFace == 4)) {
        return currentFace < lastFace;
    }
    return false;
}

bool Cube::SolveHelper(int turn, int limit, int lastFace, int cpIdx, int coIdx, int e1Idx, int e2Idx) {
    for (int move = 0; move < 18; ++move)
    {
        int face = move / 3;
		if (ShouldSkipMove(face, lastFace)) continue;

		// calculate the next state indices using the move tables
		int cpNext = CP_MoveTable[cpIdx][move];
        int coNext = CO_MoveTable[coIdx][move];
		int h_corners = corners_pdb[cpNext * 2187 + coNext];

		// early pruning: if the heuristic for the corners alone exceeds the remaining depth, skip this move
        if (turn + 1 + h_corners > limit) continue;

		// calculate the next state indices for first edges set and check heuristic
        int edges_1_next = Edges1_MoveTable[e1Idx][move];
        int h_edges1 = edges_pdb_1[edges_1_next];

		// early pruning: if the heuristic for the first edges set alone exceeds the remaining depth, skip this move
        if (turn + 1 + h_edges1 > limit) continue;

		// calculate the next state indices for second edges set and check heuristic
        int edges_2_next = Edges2_MoveTable[e2Idx][move];
        int h_edges2 = edges_pdb_2[edges_2_next];

		// early pruning: if the heuristic for the second edges set alone exceeds the remaining depth, skip this move
        if (turn + 1 + h_edges2 > limit) continue;


		// if we reach this point, it means this move is worth exploring further, so we record it in the steps array
        steps[turn] = move + 1;

		// if this move leads to a solution, return true
        if (turn + 1 == limit && h_corners == 0 && h_edges1 == 0 && h_edges2 == 0) {
            return true;
        }

		// apply move and continue searching
        if (turn + 1 < limit) {
            if (SolveHelper(turn + 1, limit, face, cpNext, coNext, edges_1_next, edges_2_next)) {
                return true;
            }
        }
    }

	return false;
}
vector<int> Cube::Solve(int limit) {

   /* Cube::CreatePdb();
    Cube().PDB_BFS_CORNERS();
    cout << "Finished corners PDB" << endl;
    Cube().PDB_BFS_EDGES_1();
    cout << "Finished edges 1 PDB" << endl;
    Cube().PDB_BFS_EDGES_2();
    cout << "Finished edges 2 PDB" << endl;*/

    Cube::LoadPdb();

    int cp = CalcCornerPermutationIndex();
    int co = CalcCornerOrientationIndex();
    int e1 = CalcEdgesIndex_1();
    int e2 = CalcEdgesIndex_2();

    int initial_h = GetHeuristic(cp, co, e1, e2);
    cout << "Initial Heuristic (Minimum steps required): " << initial_h << endl;
    cout << endl << "Solving..." << endl;

    for (int limit = initial_h; limit <= 20; limit++) {
        cout << "Searching depth " << limit << "..." << endl;
           if (SolveHelper(0, limit, -1, cp, co, e1, e2)) {
            return steps;
        }
    }

    return { 0 };
}