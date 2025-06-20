#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <ctime>

namespace fs = std::filesystem;

// Only create the refs folder (not .minigit, HEAD, or objects)
void init() {
    std::string refsPath = ".minigit/refs";

    if (fs::exists(refsPath)) {
        std::cout << "MiniGit already initialized (refs/ exists).\n";
        return;
    }

    fs::create_directory(refsPath);

    std::cout << "MiniGit partial init: refs folder created.\n";
}

// File hashing logic (same as before)
std::string getFileHash(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) return "";

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    unsigned long hash = 0;
    for (char ch : content) {
        hash = (hash * 31 + ch) % 100000;
    }

    return std::to_string(hash);
}

// Stages a file (assumes the file already exists in the working dir)
bool addFile(const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile) {
        std::cout << "File not found: " << filename << "\n";
        return false;
    }

    std::string hash = getFileHash(filename);
    std::string objectPath = ".minigit/objects/" + hash;

    if (!fs::exists(objectPath)) {
        std::ofstream outFile(objectPath);
        outFile << inFile.rdbuf();
        outFile.close();
    }

    std::ofstream staging(".minigit/staging_area", std::ios::app);
    staging << filename << " " << hash << "\n";
    staging.close();

    std::cout << "Staged " << filename << " with hash " << hash << "\n";
    return true;
}

// Helper: Get current timestamp
std::string getTimestamp() {
    time_t now = time(0);
    char buf[100];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return std::string(buf);
}

// Helper: Generate fake commit hash
std::string generateCommitHash() {
    return std::to_string(rand() % 1000000);
}

// Commit the staged files to the current branch
void commit(const std::string& message) {
    std::ifstream staging(".minigit/staging_area");
    if (!staging || staging.peek() == std::ifstream::traits_type::eof()) {
        std::cout << "Nothing to commit.\n";
        return;
    }

    std::string commitHash = generateCommitHash();
    std::string parentHash;

    std::ifstream head(".minigit/HEAD");
    std::string ref;
    std::getline(head, ref);
    ref = ref.substr(5);

    std::ifstream branchFile(".minigit/" + ref);
    std::getline(branchFile, parentHash);

    fs::create_directory(".minigit/commits");

    std::ofstream commitFile(".minigit/commits/" + commitHash);
    commitFile << "commit " << commitHash << "\n";
    commitFile << "parent " << parentHash << "\n";
    commitFile << "timestamp " << getTimestamp() << "\n";
    commitFile << "message " << message << "\n";
    commitFile << "files\n";

    std::string line;
    while (std::getline(staging, line)) {
        commitFile << line << "\n";
    }
    commitFile.close();

    std::ofstream updateBranch(".minigit/" + ref);
    updateBranch << commitHash;
    updateBranch.close();

    std::ofstream clearStaging(".minigit/staging_area");
    clearStaging.close();

    std::cout << "Committed as " << commitHash << "\n";
}

// Print all commits on the current branch
void log() {
    std::ifstream head(".minigit/HEAD");
    std::string ref;
    std::getline(head, ref);
    ref = ref.substr(5);

    std::ifstream branch(".minigit/" + ref);
    std::string currentHash;
    std::getline(branch, currentHash);

    while (!currentHash.empty()) {
        std::ifstream commitFile(".minigit/commits/" + currentHash);
        if (!commitFile) break;

        std::string line;
        while (std::getline(commitFile, line)) {
            if (line == "files") break;
            std::cout << line << "\n";
        }
        std::cout << "-------------------------\n";

        commitFile.clear();
        commitFile.seekg(0);
        while (std::getline(commitFile, line)) {
            if (line.rfind("parent ", 0) == 0) {
                currentHash = line.substr(7);
                break;
            }
        }

        if (currentHash.empty()) break;
    }
}

// Create a new branch file pointing to the current commit
void branch(const std::string& branchName) {
    std::ifstream head(".minigit/HEAD");
    std::string ref;
    std::getline(head, ref);
    ref = ref.substr(5);

    std::ifstream currentBranch(".minigit/" + ref);
    std::string currentCommit;
    std::getline(currentBranch, currentCommit);

    std::ofstream newBranch(".minigit/refs/" + branchName);
    newBranch << currentCommit;
    newBranch.close();

    std::cout << "Created branch: " << branchName << "\n";
}

// Switch current HEAD to another branch
void checkout(const std::string& branchName) {
    std::string branchPath = ".minigit/refs/" + branchName;
    if (!fs::exists(branchPath)) {
        std::cout << "Branch does not exist: " << branchName << "\n";
        return;
    }

    std::ofstream head(".minigit/HEAD");
    head << "ref: refs/" << branchName << "\n";
    head.close();

    std::cout << "Switched to branch: " << branchName << "\n";
}
