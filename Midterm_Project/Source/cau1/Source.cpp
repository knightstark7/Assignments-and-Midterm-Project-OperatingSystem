#include<iostream>
#include<fstream>
#include<string>
#include<Windows.h>
#include<wchar.h>
using namespace std;

void create_DAT_file(string file_name, int cluster_size, int byte_per_sector, int i) {
    int cluster = 4 - i % 4;
    int byte = cluster * cluster_size * byte_per_sector; // tinh kich thuoc byte tuong ung voi so cluster
    fstream f;
    f.open(file_name, ios::out);
    if (cluster == 1) { //neu cluster = 1 thi dien vo 1 dong
        f << 2020 + i << endl;
    }
    for (int j = 0; j < (byte / (4 + 2)) / 2 + 1; j++) { //giai thich trong bao cao
        f << 2020 + i << endl;
    }
    f.close();
}

int main() {

    TCHAR volume[MAX_PATH + 1] = { 0 };
    DWORD sectorsPerCluster = 0;
    DWORD bytesPerSector = 0;
    DWORD reservedSectors = 0;
    DWORD numberOfFATs = 0;
    ULARGE_INTEGER totalBytes = { 0 };
    ULARGE_INTEGER freeBytes = { 0 };
    DWORD sectorsPerFAT = 0;
    DWORD rootDirectoryEntryCount = 0;
    DWORD dataStartSector = 0;

    int N = 100;

    cout << "Enter the volume name (e.g. C:\\): ";
    wcin >> volume;


    if (!GetDiskFreeSpaceEx(volume, NULL, &totalBytes, &freeBytes))
    {
        std::cout << "Error: Failed to retrieve volume information\n";
        return 1;
    }

    if (!GetDiskFreeSpace(volume, &sectorsPerCluster, &bytesPerSector, &numberOfFATs, &sectorsPerFAT))
    {
        std::cout << "Error: Failed to retrieve volume information\n";
        return 1;
    }
    fstream f;
    //convert TCHAR to string
    string volume_path = "";
    for (int i = 0; i < MAX_PATH + 1; i++) {
        if (volume[i] == '\0') {
            break;
        }
        volume_path += volume[i];
    }
    for (int i = 0; i < N; i++) {
        string file_Path = volume_path + "F" + to_string(i) + ".Dat";
        create_DAT_file(file_Path, sectorsPerCluster, bytesPerSector, i);
    }

    return 0;
}