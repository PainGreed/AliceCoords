#include <Windows.h>
#include<TlHelp32.h>
#include <iostream>
#include <tchar.h> // _tcscmp
#include <vector>

double getdistancecoords3d(double x1, double y1, double z1, double x2, double y2, double z2) {
    return sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2) + pow((z2 - z1), 2));
}
DWORD GetModuleBaseAddress(TCHAR* lpszModuleName, DWORD pID) {
    DWORD dwModuleBaseAddress = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID); // make snapshot of all modules within process
    MODULEENTRY32 ModuleEntry32 = { 0 };
    ModuleEntry32.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(hSnapshot, &ModuleEntry32)) //store first Module in ModuleEntry32
    {
        do {
            if (_tcscmp(ModuleEntry32.szModule, lpszModuleName) == 0) // if Found Module matches Module we look for -> done!
            {
                dwModuleBaseAddress = (DWORD)ModuleEntry32.modBaseAddr;
                break;
            }
        } while (Module32Next(hSnapshot, &ModuleEntry32)); // go through Module entries in Snapshot and store in ModuleEntry32


    }
    CloseHandle(hSnapshot);
    return dwModuleBaseAddress;
}


int main() {

    HWND hGameWindow = FindWindow(NULL, "Alice: Madness Returns");
    if (hGameWindow == NULL) {
        std::cout << "Start the game!" << std::endl;
        return 0;
    }
    DWORD pID = NULL; // ID of our Game
    GetWindowThreadProcessId(hGameWindow, &pID);
    HANDLE processHandle = NULL;
    processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);
    if (processHandle == INVALID_HANDLE_VALUE || processHandle == NULL) { // error handling
        std::cout << "Failed to open process" << std::endl;
        return 0;
    }

    char gameName[] = "AliceMadnessReturns.exe";
    DWORD gameBaseAddress = GetModuleBaseAddress(_T(gameName), pID);
    DWORD offsetGameToBaseAdress = 0x010784F4;

    std::vector<DWORD> pointsOffsetsZ{ 0x270,0x84,0x50,0x4C,0x6C };
    std::vector<DWORD> pointsOffsetsX{ 0x270,0x84,0x50,0x4C,0x64 };
    std::vector<DWORD> pointsOffsetsY{ 0x270,0x84,0x50,0x4C,0x68 };

    DWORD baseAddress = NULL;
    //Get value at gamebase+offset -> store it in baseAddress
    ReadProcessMemory(processHandle, (LPVOID)(gameBaseAddress + offsetGameToBaseAdress), &baseAddress, sizeof(baseAddress), NULL);
    std::cout << "debugginfo: baseaddress = " << std::hex << baseAddress << std::endl;
    DWORD pointsAddress = baseAddress; //the Adress we need -> change now while going through offsets
    for (int i = 0; i < pointsOffsetsZ.size() - 1; i++) // -1 because we dont want the value at the last offset
    {
        ReadProcessMemory(processHandle, (LPVOID)(pointsAddress + pointsOffsetsZ.at(i)), &pointsAddress, sizeof(pointsAddress), NULL);
        //std::cout << "debugginfo: Value at offset = " << std::hex << pointsAddress << std::endl;
    }
    //pointsAddress += pointsOffsetsX.at(pointsOffsetsX.size() - 1); //Add Last offset -> done!!
    DWORD DefaultPoint = pointsAddress;
    DWORD PX = pointsAddress += pointsOffsetsX.at(pointsOffsetsX.size() - 1);
    pointsAddress = DefaultPoint;
    DWORD PY = pointsAddress += pointsOffsetsY.at(pointsOffsetsY.size() - 1);
    pointsAddress = DefaultPoint;
    DWORD PZ = pointsAddress += pointsOffsetsZ.at(pointsOffsetsZ.size() - 1);
    float sasX;
    float sasY;
    float sasZ;


    while (true) {
        Sleep(250);
        ReadProcessMemory(processHandle, (LPVOID)PX, &sasX, sizeof(sasX), NULL);
        ReadProcessMemory(processHandle, (LPVOID)PY, &sasY, sizeof(sasY), NULL);
        ReadProcessMemory(processHandle, (LPVOID)PZ, &sasZ, sizeof(sasZ), NULL);
        if (getdistancecoords3d(sasX, sasY, sasZ, -41649.18359, -120208.4844, -127365.1328) >= 50) {
            std::cout << "SASI" << std::endl;
            std::cout << sasX << sasY << sasZ <<"distance:"<< getdistancecoords3d(sasX, sasY, sasZ, -120195, -41564, -127365) << std::endl;
        }
        ReadProcessMemory(processHandle, (LPVOID)PX, &sasX, sizeof(sasX), NULL);
        //std::cout << sasX << std::endl;

    }

}