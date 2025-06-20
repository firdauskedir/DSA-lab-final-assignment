#include <iostream>
#include <string>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#endif
using namespace std;
using namespace std::filesystem;

int main(){
    string folder1 = ".minigit/staging";
    string folder2 = ".minigit/objects";

    if(!exists(folder1)){
        create_directories(folder1);
        cout << folder1 << " Has been created" << endl;
    }
    else{
        cout << folder1 << " Already exists" << endl;
    }
    if(!exists(folder2)){
        create_directories(folder2);
        cout << folder2 << " Has been created" << endl;
    }
    else{
        cout << folder2 << "Already exists" << endl;
    }

    cout << "Initialization compelete" << endl;

    #ifdef _WIN32
        SetFileAttributesA(".minigit", FILE_ATTRIBUTE_HIDDEN);
    #endif
}      