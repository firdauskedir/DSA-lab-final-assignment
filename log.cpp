#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <sstream>

using namespace std;
using namespace std::filesystem;

// Parse a commit file, return metadata and files
bool readCommit(const string& commitHash, string& parent, string& message, string& timestamp, vector<pair<string, string>>& files) {
    string path = ".minigit/commits/" + commitHash;
    ifstream commitFile(path);
    if (!commitFile.is_open()) {
        cerr << "Failed to open commit file: " << path << "\n";
        return false;
    }

    parent = "";
    message = "";
    timestamp = "";
    files.clear();
//And
    string line;
    while (getline(commitFile, line)) {
        if (line.find("parent:") == 0) {
            parent = line.substr(7);
        } else if (line.find("message:") == 0) {
            message = line.substr(8);
        } else if (line.find("timestamp:") == 0) {
            timestamp = line.substr(10);
        } else if (line.find("file:") == 0) {
            size_t firstColon = line.find(':');
            size_t secondColon = line.find(':', firstColon + 1);
            if (secondColon != string::npos) {
                string filename = line.substr(firstColon + 1, secondColon - firstColon - 1);
                string blobhash = line.substr(secondColon + 1);
                files.emplace_back(filename, blobhash);
            }
        }
    }

    return true;
}

// Read HEAD commit hash this reads the recent commit from our files HEAD
string read_HEAD() {
    ifstream headFile(".minigit/HEAD");
    if (!headFile.is_open()) return "";
    string head;
    getline(headFile, head);
    return head;
}

int main(int argc, char* argv[]) {
    if (argc < 2 || string(argv[1]) != "log") {
        cout << "Usage: ./log log\n";
        return 1;
    }

    string currentCommit = read_HEAD();
    if (currentCommit.empty()) {
        cout << "No commits found. HEAD is empty or missing.\n";
        return 1;
    }

    while (!currentCommit.empty()) {
        string parent, message, timestamp;
        vector<pair<string, string>> files;
        if (!readCommit(currentCommit, parent, message, timestamp, files)) {
            cout << "Error reading commit " << currentCommit << "\n";
            break;
        }

        cout << "Commit: " << currentCommit << "\n";
        cout << "Timestamp: " << timestamp << "\n";
        cout << "Message: " << message << "\n";
        cout << "Parent: " << (parent.empty() ? "(none)" : parent) << "\n";
        cout << "Files:\n";
        for (const auto& f : files) {
            cout << "- " << f.first << " (blob: " << f.second << ")\n";
        }
        cout << "----------------------------------------\n";

        currentCommit = parent;
    }

    return 0;
}