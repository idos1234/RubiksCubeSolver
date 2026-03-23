#include <windows.h>

class PDBReader {
    unsigned char* data;
    HANDLE hFile, hMap;
public:
    PDBReader(const char* filename) : data(nullptr), hFile(INVALID_HANDLE_VALUE), hMap(NULL) {
        hFile = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE) return;

        hMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
        if (hMap == NULL) {
            CloseHandle(hFile);
            return;
        }

        data = (unsigned char*)MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
    }

    inline unsigned char GetDistance(long long index) {
        if (!data) return 255;
        return data[index];
    }


    ~PDBReader() {
        UnmapViewOfFile(data);
        CloseHandle(hMap);
        CloseHandle(hFile);
    }
};