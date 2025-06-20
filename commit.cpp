#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

using namespace std;
using namespace std::filesystem;

// Simple custom hash function (djb2 variant) to hash our contents into strings
string simple_hash(const string& data) {
    unsigned long hash = 5381;
    for (char c : data) {
        hash = ((hash << 5) + hash) + c;
    }
    stringstream ss;
    ss << hex << hash;
    return ss.str();
}

// Compute hash of a file's contentst to see what has changed
string hash_file(const path& filepath) {
    ifstream file(filepath, ios::binary);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file for hashing: " << filepath << "\n";
        return "";
    }
    stringstream buffer;
    buffer << file.rdbuf();
    return simple_hash(buffer.str());
}

// Save blob to .minigit/objects/ with filename as hash so we can compare them with their previous counterparts
bool save_blob(const path& filepath, const string& hash) {
    create_directories(".minigit/objects");
    string objectPath = ".minigit/objects/" + hash;
    if (exists(objectPath)) {
        return true;
    }
    ifstream inFile(filepath, ios::binary);
    if (!inFile.is_open()) {
        cerr << "Error: Unable to open file for blob saving: " << filepath << "\n";
        return false;
    }
    ofstream outFile(objectPath, ios::binary);
    if (!outFile.is_open()) {
        cerr << "Error: Unable to create blob file: " << objectPath << "\n";
        return false;
    }
    outFile << inFile.rdbuf();
    return true;
}

// Read HEAD commit hash
string read_HEAD() {
    ifstream headFile(".minigit/HEAD");
    if (!headFile.is_open()) return "";
    string head;
    getline(headFile, head);
    return head;
}

// Update HEAD to new commit hash
void update_HEAD(const string& commitHash) {
    ofstream headFile(".minigit/HEAD", ios::trunc);
    if (!headFile.is_open()) {
        cerr << "Error: Unable to update HEAD file.\n";
        return;
    }
    headFile << commitHash << "\n";
}

int main(int argc, char* argv[]) {
    //This will create these directories IF AND ONLY IF they are not created from the minigit initialization
    try {
        if (!exists(".minigit"))
            create_directory(".minigit");
        if (!exists(".minigit/staging"))
            create_directory(".minigit/staging");
        if (!exists(".minigit/objects"))
            create_directory(".minigit/objects");
        if (!exists(".minigit/commits"))
            create_directory(".minigit/commits");
    } catch (const filesystem_error& e) {
        cerr << "Filesystem error creating directories: " << e.what() << "\n";
        return 1;
    }

    if (argc < 4 || string(argv[1]) != "commit" || string(argv[2]) != "-m") {
        cout << "Usage: ./minigit commit -m \"commit message\"\n";
        return 1;
    }
    string commitMsg = argv[3];

    // Read parent commit hash
    string parentCommit = read_HEAD();

    // Read staged files and compute hashes + save blobs
    vector<pair<string, string>> files; // pair<filename, blobhash>

    for (const auto& entry : directory_iterator(".minigit/staging")) {
        if (is_regular_file(entry.path())) {
            string filename = entry.path().filename().string();
            string blobHash = hash_file(entry.path());
            if (blobHash.empty()) {
                cerr << "Failed to hash file: " << filename << "\n";
                return 1;
            }
            if (!save_blob(entry.path(), blobHash)) {
                cerr << "Failed to save blob for file: " << filename << "\n";
                return 1;
            }
            files.emplace_back(filename, blobHash);
        }
    }

    if (files.empty()) {
        cout << "Nothing to commit. Staging area is empty.\n";
        return 0;
    }

    // Prepare commit content string for hashing (to get commit hash)
    auto now = chrono::system_clock::now();
    time_t t = chrono::system_clock::to_time_t(now);
    string timestamp = ctime(&t);
    timestamp.pop_back(); // Remove trailing newline

    stringstream commitContent;
    commitContent << "parent:" << parentCommit << "\n";
    commitContent << "message:" << commitMsg << "\n";
    commitContent << "timestamp:" << timestamp << "\n";
    for (auto& f : files) {
        commitContent << "file:" << f.first << ":" << f.second << "\n";
    }

    // Compute commit hash
    string commitHash = simple_hash(commitContent.str());

    // Save commit object file
    string commitPath = ".minigit/commits/" + commitHash;
    ofstream commitFile(commitPath);
    if (!commitFile.is_open()) {
        cerr << "Error: Unable to create commit file.\n";
        return 1;
    }
    commitFile << commitContent.str();
    commitFile.close();

    // Update HEAD so the parent and child commit is
    update_HEAD(commitHash);

    cout << "Committed as " << commitHash << " with message: \"" << commitMsg << "\"\n";

    return 0;
}