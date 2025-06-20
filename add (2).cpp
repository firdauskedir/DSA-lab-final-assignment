#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;
using namespace std::filesystem;

string simple_hash(const string& data) {
    unsigned long hash = 5381;
    for (char c : data) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    stringstream ss;
    ss << hex << hash;
    return ss.str();
}

string hash_file(const path& filepath) {
    ifstream file(filepath, ios::binary);
    if (!file.is_open()) {
        cerr << "Error: Cannot open file: " << filepath << endl;
        return "";
    }
    stringstream buffer;
    buffer << file.rdbuf();
    return simple_hash(buffer.str());
}

void addFile(const string& filename) {
    if (!exists(filename)) {
        cout << "File not found: " << filename << endl;
        return;
    }

    string blobHash = hash_file(filename);
    if (blobHash.empty()) {
        cout << "Failed to hash file" << endl;
        return;
    }

    create_directories(".minigit/objects");
    string blobPath = ".minigit/objects/" + blobHash;

    if (!exists(blobPath)) {
        ifstream inFile(filename, ios::binary);
        ofstream outFile(blobPath, ios::binary);
        outFile << inFile.rdbuf();
        cout << "Saved blob: " << blobPath << endl;
    } else {
        cout << "Blob already exists: " << blobPath << endl;
    }

    create_directories(".minigit/staging");
    string stagedPath = ".minigit/staging/" + path(filename).filename().string();
    copy_file(filename, stagedPath, copy_options::overwrite_existing);
    cout << "Staged file: " << stagedPath << endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: ./minigit add <filename>" << endl;
        return 1;
    }
    addFile(argv[1]);
    return 0;
}








